#Convert all HOC files in a folder to CNG files
#Author: Ruben Ascoli, rascoli@princeton.edu, undergraduate, Princeton University
#George Mason University Department of Bioengineering

#Failures: weird formatting or not using pt3dadd method
import os
import sys
import pdb
# from .sort import swc_sort

def hoc2swc(filename, outputfilename):

	FAILURE = False

	try:
		hocfile = open(filename,'r')
		hoclines = hocfile.readlines() #read contents of hoc file, line by line
		cnglines = [] #will write these lines to the CNG file
		count = 0 #total number of lines
		tag = 0 #kind of branch
		branchname = "" #current name of branch, such as "dend[0]" for the first element in the list dend.
		startDict = {} #line of start of the list
		newBranch = False #tells whether we just started entering lines for a new branch
		endDict = {} #line of end of the list
		searchBracket = False #tells whether to search '{' as new branch creation
		if len([line for line in hoclines if line.find('access')!=-1])<=len([line for line in hoclines if line.find('pt3dclear')!=-1])-2: #'access' shows up less than 'pt3dclear'
			searchBracket=True
		if len([line for line in hoclines if line.find('access')!=-1])<=len([line for line in hoclines if line.find('pt3dadd')!=-1])/1000:
			searchBracket=True
		if len([line for line in hoclines if line.find('access')!=-1])==1 and len([line for line in hoclines if line.find('pt3dclear')!=-1])<=2: #very small files
			searchBracket=True
		for line in hoclines:
			if line[0:2] in ['//','/*']:
				continue
			if line.find('access')!=-1 or (searchBracket and line.find('{')!=-1 and (line.find('soma')!=-1 or line.find('axon')!=-1 or line.find('basal')!=-1 or line.find('dend')!=-1 or line.find('apical')!=-1 or line.find('section')!=-1 or line.find('filament')!=-1 or line.find('apic')!=-1 or line.find('my')!=-1)):
				if branchname!="": #we just finished up a branch
					endDict[branchname]=count
				if line.find('soma')!=-1:
					tag=1
				elif line.find('axon')!=-1:
					tag=2
				elif line.find('apic')!=-1:
					tag=4
				else:
					tag=3
				if line.find('access')!=-1:
					if line.find('}')!=-1: #need this if statement because of formatting issues
						branchname = line[line.find('access')+7:line.find('}')]
					else:
						branchname = line[line.find('access')+7:-1]
				else:
					branchname = line[:line.find('{')-1].lstrip()
				if branchname not in startDict.keys():
					newBranch=True
			if line.find('pt3dadd')!=-1:
				count+=1
				if newBranch:
					startDict[branchname]=count
				s = str(count)+" "+str(tag)+" "
				commacount=0
				charindex = line.find('(')+1 #start after left parenthesis
				toadd = ""
				while (commacount<3):
					if line[charindex] in '-.0123456789':
						toadd+=line[charindex]
					elif line[charindex]=='e':
						charindex = line.find(',',charindex)-1
						toadd = '0'
					elif line[charindex]==',':
						s+=toadd+" "
						commacount+=1
						toadd=""
					charindex+=1
				if line.find(',',charindex)!=-1:
					s+=str(float(line[charindex:line.find(',',charindex)])/2)+" "
				else:
					s+=str(float(line[charindex:line.find(')')])/2)+" "
				#connections
				if newBranch==False:
					s+=str(count-1) #connect to previous
				else:
					s+="0" #for now, don't connect to anything - will resolve in the second pass.
				s+="\n"
				newBranch=False
				cnglines.append(s)
		if branchname not in endDict:
			endDict[branchname]=count
		#second pass creates connections between branches, from "fro" to "to"
		for line in hoclines:
			if line[0:2] in ['//','/*']:
				continue
			conind=line.find('connect') #index of 'connect' in line
			if conind==-1:
				continue
			if line[conind+7]!=' ':
				continue
			if conind!=0 and line[conind-1]!=' ':
				continue
			if line.find('node')!=-1:
				continue #ignore connections with nodes
			if line.find('for ')==-1: #no for loops - regular thing
				findparen = line.find('(',conind+10)
				if findparen==-1:
					findparen=999999
				findspace = line.find(' ',conind+10)
				if findspace==-1:
					findspace=999999
				branchnameTo=line[conind+8:min(findparen,findspace)].lstrip()
				if branchnameTo not in startDict:
					print("Couldn't find branch: "+branchnameTo)
					FAILURE=True
					break
				position = float(line[line.find('(')+1:line.find(')')])
				to = startDict[branchnameTo]+int(position*(endDict[branchnameTo]-startDict[branchnameTo]))
				#due to formatting issues, branchnameFro could appear before or after 'connect'
				branchnameFro = ""
				position = -1.0
				if conind>=2: #search for branchnameFro before 'connect'
					for i in range(1,conind-1):
						if line[conind-1-i:conind-1] in startDict:
							branchnameFro=line[conind-1-i:conind-1] #found it
							if line.find('}')!=-1: #need this if statement because of formatting issues
								position = float(line[line.find(',',conind)+2:line.find('}')])
							else:
								position = float(line[line.find(',',conind)+2:-1])
				if branchnameFro=="": #search for branchnameFro after 'connect' if didn't find it before
					i = 1
					while ((line[line.find(',',conind)+2:line.find(',',conind)+2+i]) not in startDict or line[line.find(',',conind)+2+i]=='[') and i<1000:
						i+=1
					if i==1000: #not a real connection
						continue
					branchnameFro = line[line.find(',',conind)+2:line.find(',',conind)+2+i]
					position = float(line[line.find('(',line.find(',',conind))+1:line.find(')',line.find(',',conind))])
				fro = startDict[branchnameFro]+int(position*(endDict[branchnameFro]-startDict[branchnameFro]))
				cnglines[to-1] = cnglines[to-1][:cnglines[to-1].rindex(" ")+1]+str(fro)+"\n"
			else:
				start = int(line[line.find("=")+1:line.find(',')])
				end = int(line[line.find(",")+1:line.find(" ",line.find(',')+2)])
				for i in range(start,end+1): #form the connections
					findparen = line.find('(',conind+10)
					if findparen==-1:
						findparen=999999
					findspace = line.find(' ',conind+10)
					if findspace==-1:
						findspace=999999
					branchnameTo=line[conind+8:min(findparen,findspace)]
					indToReplace = branchnameTo.find("[i]")
					if indToReplace!=-1:
						branchnameTo = branchnameTo[:indToReplace]+"["+str(i)+"]"+branchnameTo[indToReplace+3:]
					else:
						print("we have an unusual circumstance. "+line)
					if branchnameTo not in startDict:
						print("Couldn't find branch: "+branchnameTo+" in for loop: "+line)
						FAILURE=True
						break
					pos = float(line[line.find('(')+1:line.find(')')])
					to = startDict[branchnameTo]+int(position*(endDict[branchnameTo]-startDict[branchnameTo]))
					#due to formatting issues, branchnameFro could appear before or after 'connect'
					branchnameFro = ""
					position = -1.0
					if conind>=2: #search for branchnameFro before 'connect'
						for j in range(1,conind-1):
							potentialBranchNameFro = line[conind-1-j:conind-1]
							indToReplace = potentialBranchNameFro.find("[i-1]")
							if indToReplace!=-1:
								potentialBranchNameFro=potentialBranchNameFro[:indToReplace]+"["+str(i-1)+"]"+potentialBranchNameFro[indToReplace+5:]
							if potentialBranchNameFro in startDict:
								branchnameFro=potentialBranchNameFro #found it
								if line.find('}')!=-1: #need this if statement because of formatting issues
									position = float(line[line.find(',',conind)+2:line.find('}')])
								else:
									position = float(line[line.find(',',conind)+2:-1])
					if branchnameFro=="": #search for branchnameFro after 'connect' if didn't find it before
						for j in range(1000):
							if (len(line)>line.find(',',conind)+2+j and line[line.find(',',conind)+2+j]=='['):
								continue
							potentialBranchNameFro=line[line.find(',',conind)+2:line.find(',',conind)+2+j]
							indToReplace = potentialBranchNameFro.find("[i-1]")
							if indToReplace!=-1:
								potentialBranchNameFro=potentialBranchNameFro[:indToReplace]+"["+str(i-1)+"]"+potentialBranchNameFro[indToReplace+5:]
								if potentialBranchNameFro[-3:]=='[0]' and potentialBranchNameFro not in startDict and potentialBranchNameFro[:-3] in startDict:
									potentialBranchNameFro = potentialBranchNameFro[:-3]
									j-=3
							if potentialBranchNameFro in startDict:
								branchnameFro=potentialBranchNameFro
								break
						if j==1000: #not a real connection
							continue
						position = float(line[line.find('(',line.find(',',conind))+1:line.find(')',line.find(',',conind))])
					fro = startDict[branchnameFro]+int(position*(endDict[branchnameFro]-startDict[branchnameFro]))
					choppedCNGLine = cnglines[to-1][:cnglines[to-1].rindex(" ")+1]
					cnglines[to-1] = choppedCNGLine+str(fro)+"\n"
		#At this point there is still one point whose parent is the placeholder "0"; this is the root.
		#If this is not the case, there is a loop! I will break it at an arbitrary point.
		foundzero = False
		for i in range(len(cnglines)):
			if int(cnglines[i].split(" ")[-1])==0:
				cnglines[i]=cnglines[i][:-2]+"-1\n"
				foundzero=True
		if foundzero==False:
			cnglines[0] = cnglines[0][:cnglines[0].rindex(" ")+1]+"-1\n"

		hocfile.close()

	except:
		FAILURE=True

	finally:
		if FAILURE==False:
			if outputfilename is None:
				outputfilename = os.path.splitext(filename)[0] + '.swc'
			cngfile = open(outputfilename,'w')
			cngfile.writelines(cnglines)
			cngfile.close()
			# swc_sort(outputfilename, outputfilename)
			return 'SUCCESS'
		else:
			print("Conversion ERROR: Failed on file: "+filename)
			# failedfiles.append(filename)
			hocfile.close()
			return 'FAIL'
