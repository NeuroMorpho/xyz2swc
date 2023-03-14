#coding=utf-8
import sys
import os
import argparse
try:
    from lxml import etree
except ImportError:
    raise ImportError('lxml library is required')
try:
    from cStringIO import StringIO
except ImportError:
    try:
        from io import StringIO
    except:
        from StringIO import StringIO
try:
    from sys import maxint as maxint
except:
    from sys import maxsize as maxint
from copy import deepcopy
from zipfile import ZipFile
__doc__='''
this module helps convert nml or nmx format files to swc format files.
there are two ways to use this module:
(1) python somefile.nml(or somefile.nmx) [-o output file name or directory] [--radius radius of swc node default is 1.0]
(2) use this module in your project:
    from  nml2swc import parseFile
    parseFile(somefile.nml(or somefile.nmx),output file name or directory,radius)
'''
class NmlParser(object):
    def __init__(self,radius,contents):
        super(NmlParser,self).__init__()
        self.processed=[]
        self.id=1
        self.contents=contents
        self.radius=radius
        self.result=[]
        self.nml=[]
        self.nodes={}

    def process(self):
        try:
            max_recursion=sys.getrecursionlimit()
            self._createGraph()
            self._getParamsAndComments()
            self._dfs()
        except RuntimeError:
            sys.setrecursionlimit(max_recursion*10)
            self._clear()
            self.process()
            sys.setrecursionlimit(max_recursion)
        return self.result

    def _clear(self):
        self.processed=[]
        self.id=1
        self.result=[]
        self.nml=[]
        self.nodes={}

    def _createGraph(self):
        for content in self.contents:
            try:
                # breakpoint()
                if type(content['string']) == bytes:
                    self.nml.append((etree.fromstring(content['string'].decode().encode('utf-8')), content['kind']))
                else:
                    self.nml.append((etree.fromstring(content['string'].encode('utf-8')), content['kind']))
            except etree.XMLSyntaxError:
                continue
        for nml in self.nml:
            for node in nml[0].xpath(r'//node'):
                self.nodes[node.get('id')]={'x':node.get('x'),'y':node.get('y'),'z':node.get('z'),
                'visited':False,'index':maxint,'t':nml[1],'child':[]}
        for nml in self.nml:
            for edge in nml[0].xpath(r'//edge'):
                s=edge.get('source')
                t=edge.get('target')
                self.nodes[s]['child'].append(t)
                self.nodes[t]['child'].append(s)

    def _dfs(self):
        for node,attr in self.nodes.items():
            if not attr['visited']:
                self._dfs_(node,attr,-1)

    def _dfs_(self,node,attr,parent):
        attr['visited']=True
        attr['index']=self.id
        self.id+=1
        self._outputNode(attr,parent)
        for c in attr['child']:
            cattr=self.nodes[c]
            #recursively visit children
            if not cattr['visited']:
                self._dfs_(c,cattr,attr['index'])
            #handle loop when traceback
            else:
                if cattr['index']>attr['index']:
                    nattr=deepcopy(cattr)
                    nattr['index']=self.id
                    self.id+=1
                    self._outputNode(nattr,attr['index'])

    def _outputNode(self,attr,parent):
        record="{id} {t} {x} {y} {z} {r} {p}".format(id=attr['index'],
        x=attr['x'],y=attr['y'],z=attr['z'],t=attr['t'],r=self.radius,p=parent)
        self.result.append(record)

    def _getParamsAndComments(self):
        for nml in self.nml:
            for param in nml[0].xpath(r'//parameters'):
                try:
                    p=StringIO(etree.tostring(param).decode('utf-8'))
                except:
                    p=StringIO(etree.tostring(param))
                for line in p.readlines():
                    self.result.append('#'+line)
            for com in nml[0].xpath(r'//comments'):
                try:
                    c=StringIO(etree.tostring(com).decode('utf-8'))
                except:
                    c=StringIO(etree.tostring(com))
                for line in c.readlines():
                    self.result.append('#'+line)


def write2File(result,fname):
    with open(fname,'w') as f:
        for item in result:
            f.write(item)
            if not item.endswith('\n'):
                f.write('\n')
        f.flush()


def getOutputName(output,name):
    rv=output
    if not rv.endswith('.swc'):
        if not os.path.isdir(rv):
            os.makedirs(rv)
        rv=os.path.join(rv,os.path.split(name)[1][:-4]+'.swc')
    return rv


def nml2swc(filename,output,radius=1.0):
    if not os.path.isfile(filename):
        print('Error: {0} does not exists!'.format(filename))
        return 'FAIL'
    of=getOutputName(output,filename)
    if filename.lower().endswith('.nml'):
        with open(filename,'r') as f:
            result=NmlParser(radius,({'string':f.read(),'kind':0},)).process()
            write2File(result,of)
            # print('parse {0} done ,result saved at {1}'.format(filename,of))
            if os.stat(of).st_size == 0:
                return 'FAIL'
            else:
                return 'SUCCESS'
    elif filename.lower().endswith('.nmx'):
        z=ZipFile(filename,'r')
        contents=[]
        for f in z.namelist():
            s=z.open(f)
            d,f=os.path.split(f)
            if len(f.split('_'))>=4 and f.split('_')[3]=='soma':
                contents.append({'string':s.read(),'kind':1})
            elif len(f.split('_'))>=4 and f.split('_')[3]=='skeleton':
                contents.append({'string':s.read(),'kind':0})
            else:
                print('Warning! {0} contains invalid file name {1}'.format(filename,f))
                continue
        if contents:
            result=NmlParser(radius,contents).process()
            write2File(result,of)
            # print('parse {0} done ,result saved at {1}'.format(filename,of))
            return 'SUCCESS'
    else:
        print('Error: Invalid input file format (nml or nmx format required!)')
        return 'FAIL'

# if __name__=='__main__':
#     parser=argparse.ArgumentParser(description=__doc__)
#     parser.add_argument('filename')
#     parser.add_argument('-o',action='store',default=r'./',help='output file name or directory')
#     parser.add_argument('--radius',action='store',default=1.0,type=float,help='radius default=1.0')
#     args=parser.parse_args()
#     parseFile(args.filename,args.o,args.radius)
