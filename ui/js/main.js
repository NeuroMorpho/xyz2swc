var filelist = document.getElementById('filetable');
const remoteurl = '..';
// generate random folder name
var remotefolder = Math.random().toString(20).slice(2);

var progress_success = 0;
var progress_failed = 0;

var filesdm = {
  get: function (id) {
    return this.data[id];
  },
  del: function (id) {
    delete this.data[id];
    AjaxRequest.send('/events/delete/' + id);
  },
  data:{
  },
  addfiles: function(inputfiles) {

    // read all files that are zip files, and add to a stack, then read all files that are
    // not zip files
    var zipfiles = [];
    var otherfiles = [];
    for (const afile of inputfiles) {
      if (afile.name.endsWith('.zip')) {
        zipfiles.push(afile);
      }
      else {
        otherfiles.push(afile);
      }
    }
    // extract all zip files and for each file call addfile(), then after resolved promise, read all other files and add them
    var zippromises = [];
    for (const afile of zipfiles) {
      zippromises.push(new Promise((resolve, reject) => {
        JSZip.loadAsync(afile).then((zip) => {
        
          for (const [filename, file] of Object.entries(zip.files)) {
            if ((!file.dir) && (validFileType(file))) {
              file.async('blob').then((content) => {
                // only keep filename, skip path
                thisfile = filename.split('/').pop();
                var newfile = new File([content], thisfile, {type: file.options.type});
                filesdm.addfile(newfile);
              });
            }
          }
          resolve();
        });
      }));


    }
    Promise.all(zippromises).then(() => {
      for (const afile of otherfiles) {
        filesdm.addfile(afile);
      }
    });



  },
  addfile: function(item) {
      this.data[item.name] = {
        file: item,
        size: item.size,
        converted:'Not started',
        checked: 'Not started'
      };
      updateFileList();
    }
  ,
  updatestatus: function(filename,status) {
    this.data[filename].converted = status;
  },
  updatechk: function(filename,status) {
    this.data[filename].checked = status;
  }
}

const preview = document.querySelector('.preview');


const fileTypes = [
    "dat","asc","ims","swc","hoc","nml","mtr","ims","nmx","mat","traces","ndf","am","stl","vtk"
];
  
function validFileType(file) {
  thistype = file.name.split('.').pop();
  return fileTypes.includes(thistype.toLowerCase());
}

function returnFileSize(number) {
  if(number < 1024) {
    return number + 'bytes';
  } else if(number >= 1024 && number < 1048576) {
    return (number/1024).toFixed(1) + 'KB';
  } else if(number >= 1048576 && number < 1073741824) {
    return (number/1048576).toFixed(1) + 'MB';
  } 
  else if(number >= 1073741824) {
     return (number/1073741824).toFixed(1) + 'GB';
  }

}

//input.style.opacity = 0;

function clearlist() {
  filesdm.data = {};
  filelist.innerHTML = '';
  remotefolder = Math.random().toString(20).slice(2);
  document.getElementById('fileselector').value = '';

}

function sendConvert () {
  // (A) GET FORM DATA
  
//  for (const afile of document.getElementById("fileselector").files) {
  for (const akey in filesdm.data) {
    var data = new FormData();
    afile = filesdm.data[akey].file;
    convButton = document.getElementById(`bstatus_${afile.name}`);
    convButton.innerText = 'Waiting';
    filesdm.updatestatus(afile.name,'Waiting');
    convButton.setAttribute('class','btn btn-warning disabled');
    data.append('files', afile, afile.name);
    

    data.append('folder',remotefolder)

    // (B) INIT FETCH POST
    fetch(remoteurl + `/convertfiles?folder=${remotefolder}`, {
      method: "POST",
      body: data
    })
  
    // (C) RETURN SERVER RESPONSE AS TEXT
    .then((result) => {
      if (result.status != 200) {
        // update failed progress bar
        
        throw new Error("Bad Server Response"); 
      }
      return result.text();
    })
  
    // (D) SERVER RESPONSE
    .then((response) => {
      response = JSON.parse(response)['data'];
      console.log(response);
      remotefolder = response['folder'];
      for (let afile in response["converted"]) {
        console.log(afile);
        convButton = document.getElementById(`bstatus_${afile}`);
        checkbutton = document.getElementById(`chkstatus_${afile}`);
        if (response["converted"][afile]['status'] == 'SUCCESS') {
          convButton.innerText = 'Converted';
          filesdm.updatestatus(afile,'Converted');
          convButton.setAttribute('class','btn btn-success');
          checkbutton.setAttribute('class','btn btn-success');
          checkbutton.innerText = 'Checked';
          checkbutton.setAttribute('data-bs-content',`<div><div>${response["converted"][afile]["report"]}</div></div>`.replace(/[\t\n]/g,"").replace(/"/g,"'"));
          // update success progress bar
          progress_success += 1;
          nFiles = Object.keys(filesdm.data).length;
          var progressbar = document.getElementById('progress-success');
          progressbar.setAttribute('style',`width: ${progress_success/nFiles*100}%;`);
        }
        else {
          convButton.innerText = 'Failed';
          filesdm.updatestatus(afile,'Failed');
          convButton.setAttribute('class','btn btn-danger');
          checkbutton.setAttribute('class','btn btn-danger');
          checkbutton.innerText = 'Failed';
          checkbutton.setAttribute('data-bs-content','<div><div><b>Bad Server Response</div></div>');
          // update failed progress bar
          progress_failed += 1;
          nFiles = Object.keys(filesdm.data).length;
          var progressbar = document.getElementById('progress-failed');
          progressbar.setAttribute('style',`width: ${progress_failed/nFiles*100}%;`);
        }

        

      }
    })
    .catch((error) => {
/*       convButton = document.getElementById(`bstatus_${afile.name}`);
      checkbutton = document.getElementById(`chkstatus_${afile.name}`);
      convButton.innerText = 'Failed';
      filesdm.updatestatus(afile.name,'Failed');
      convButton.setAttribute('class','btn btn-danger');
      checkbutton.setAttribute('class','btn btn-danger');
      checkbutton.innerText = 'Failed';
      checkbutton.setAttribute('data-bs-content',`<div><div><b>${error}</b></div></div>`); */
      // update failed progress bar
      progress_failed += 1;
      nFiles = Object.keys(filesdm.data).length;
      var progressbar = document.getElementById('progress-failed');
      progressbar.setAttribute('style',`width: ${progress_failed/nFiles*100}%;`);
      console.log(error);
    })
  }
  

  // (E) PREVENT FORM SUBMIT
  return false;
}

function fileinput() {
  var input = document.getElementById('fileselector');
  filesdm.addfiles(input.files);
}

function sendCheck () {
  // (A) GET FORM DATA
  
  for (const afile of document.getElementById("fileselector").files) {
    var data = new FormData();
    checkbutton = document.getElementById(`chkstatus_${afile.name}`);
    checkbutton.innerText = 'Waiting';
    filesdm.updatestatus(afile.name,'Waiting');
    checkbutton.setAttribute('class','btn btn-warning disabled');
    data.append('files', afile, afile.name);
    

    data.append('folder',remotefolder)
  
    // (B) INIT FETCH POST
    fetch(remoteurl + `/checkfiles?folder=${remotefolder}`, {
      method: "POST",
      body: data
    })
  
    // (C) RETURN SERVER RESPONSE AS TEXT
    .then((result) => {
      if (result.status != 200) { throw new Error("Bad Server Response"); }
      return result.text();
    })
  
    // (D) SERVER RESPONSE
    .then((response) => {
      response = JSON.parse(response)['data'];
      console.log(response);
      remotefolder = response['folder']
      for (let afile in response["checked"]) {
        chkstatus = response["checked"][afile]["status"]
        console.log(afile)
        checkbutton = document.getElementById(`chkstatus_${afile}`)
        if (chkstatus == 'FAIL') {
          checkbutton.innerText = 'Failed'
          filesdm.updatechk(afile,'Failed');
          checkbutton.setAttribute('class','btn btn-danger');
        }
        else {
          checkbutton.innerText = 'Success'
          filesdm.updatechk(afile,'Success');
          checkbutton.setAttribute('class','btn btn-success');
        }
        checkbutton.setAttribute('data-bs-content',`<div><div>${response["checked"][afile]["report"]}</div></div>`.replace(/[\t\n]/g,"").replace(/"/g,"'"));
      }
    })
  
    // (E) HANDLE ERRORS - OPTIONAL
    .catch((error) => { console.log(error); });
  }
 
  // (F) PREVENT FORM SUBMIT
  return false;
}


function getzipped(zipurl='/getzipped') {
  fetch(remoteurl + zipurl + `/${remotefolder}`, {
    method: "get",
  })
  .then( res => res.blob() )
  .then( blob => {
    var file = window.URL.createObjectURL(blob,{type: "application/zip"});
    const a = document.createElement('a')
    a.style.display = 'none'
    a.href = file
    a.download = Date()+".zip"
    document.body.appendChild(a)
    a.click()

  });
  
}

function updateFileList() {
  while(preview.firstChild) {
    preview.removeChild(preview.firstChild);
  }
  // Empty table
  filelist.innerHTML = '';

  // add selected files to model


  const curFiles = filesdm.data;
  if(curFiles.length === 0) {
    const para = document.createElement('p');
    para.textContent = 'No files currently selected for upload';
    preview.appendChild(para);
  } else {
      
    rownum = 0;
    for(const afile in curFiles) {
      rownum+=1;
      var row = document.createElement("tr");
      filelist.appendChild(row);
      const headerItem = document.createElement('th');
      const listItem = document.createElement('td');
      const sizeItem = document.createElement('td');
      const convItem = document.createElement('td');
      const checkItem = document.createElement('td');
      convButton = document.createElement('button');
      convButton.setAttribute('type','button');
      convButton.setAttribute('id',`bstatus_${afile}`);
      convButton.innerText = curFiles[afile].converted;
      switch (curFiles[afile].converted) {
        case 'Waiting':
          attrib = 'btn btn-warning disabled';
          break;
        case 'Checked': 
          attrib = 'btn btn-success';
          break;
        case 'Failed':
          attrib = 'btn btn-danger disabled';
          break;
        default:
          attrib = 'btn btn-primary';
      }
      convButton.setAttribute('class',attrib);
      convItem.appendChild(convButton);
      headerItem.innerHTML = rownum;
      ending = afile.split('.').pop();
      if  (fileTypes.includes(ending.toLowerCase())) {
        listItem.innerHTML = afile;
        sizeItem.innerHTML = returnFileSize(curFiles[afile].size);

      } else {
        listItem.innerHTML = `File name ${afile}: Not a valid file type. Update your selection.`;
      }

      checkbutton = document.createElement('button');
      checkbutton.setAttribute('type','button');
      checkbutton.setAttribute('data-bs-toggle','popover');
      checkbutton.setAttribute('title','Check report');
      checkbutton.setAttribute('data-bs-content',`<div><table><tr><td>Not checked yet</td></tr></table></div>`);

      checkbutton.setAttribute('id',`chkstatus_${afile}`);
      checkbutton.innerText = curFiles[afile].checked;
      switch (curFiles[afile].checked) {
        case 'Waiting':
          attrib = 'btn btn-warning disabled';
          break;
        case 'Checked':
          attrib = 'btn btn-success';
          break;
        case 'Failed':
          attrib = 'btn btn-danger';
          break;
        default:
          attrib = 'btn btn-primary';
      }1
      checkbutton.setAttribute('class',attrib);
      var popover = new bootstrap.Popover(checkbutton,{html: true,sanitize: false});      

      checkItem.appendChild(checkbutton);


      row.appendChild(headerItem);
      row.appendChild(listItem);
      row.appendChild(sizeItem);
      row.appendChild(convItem);
      row.appendChild(checkItem);
      
    }
  }
}
  