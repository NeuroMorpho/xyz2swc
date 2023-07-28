import json
from fastapi import FastAPI, status, Request, File, UploadFile, Response
from fastapi.staticfiles import StaticFiles
from typing import Any, List, Dict, Optional
from fastapi.responses import JSONResponse, FileResponse
from pydantic.main import BaseModel
from xyz2swc import convert, standard
from fastapi.exceptions import RequestValidationError
from fastapi.encoders import jsonable_encoder
from fastapi.middleware.cors import CORSMiddleware
import string
import random
import os
import zipfile
import io
from fastapi.logger import logger
from fastapi.openapi.utils import get_openapi


# define openapi schema
def my_schema():
    if app.openapi_schema:
        return app.openapi_schema
    openapi_schema = get_openapi(
        title="xyz2swc API",
        version="1.01",
        description="Overview - xyz2swc API",
        routes=app.routes,
    )
    # add tags_metadata to openapi schema

    openapi_schema["servers"] = [
        {
            "url": "https://neuromorpho.org/xyz2swc",
            "description": "Xyz2swc API",
        }
    ]

    openapi_schema["info"] = {
        "title": "xyz2swc API",
        "version": "1.0",
        "description": "Overview - xyz2swc API",
        #        "termsOfService": "https://neuromorpho.org/useterm.jsp",
        "contact": {
            "name": "Get Help with this API",
            #            "url": "https://neuromorpho.org/myfaq.jsp",
            "email": "nmadmin@gmu.edu",
        },
        "license": {
            "name": "MIT License",
        },
    }
    openapi_schema["tags"] = [
        {
            "name": "main",
            "description": "Check that API is alive",
        },
        {
            "name": "checkfiles",
            "description": """Check files for compliance with xyz2swc
            standards. Folder parameter is optional. If not specified,
            a random folder name will be generated.""",
        },
        {
            "name": "convertfiles",
            "description": """Convert files to SWC format. Folder parameter
            is optional. If not specified, a random folder name will be
            generated.""",
        },
        {
            "name": "getzipped",
            "description": """Download converted files as a zipped archive.
            Folder parameter is mandatory, and should be the same as the
            one used in the convertfiles/checkfiles call.""",
        },
        {
            "name": "getlogs",
            "description": """Download log files as a zipped archive.
            Folder parameter is mandatory, and should be the same as
            the one used in the convertfiles/checkfiles call.""",
        },
    ]
    app.openapi_schema = openapi_schema
    return app.openapi_schema


logger.info("Booted up")


app = FastAPI(root_path="/xyz2swc")
app.openapi = my_schema


app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


class Fileres(BaseModel):
    status: str  # Status of performed action
    report: str  # HTML Report of performed action


class Convertresponse(BaseModel):
    folder: str  # Folder where files are stored
    converted: Dict[str, Fileres]  # Dictionary of converted files


class Checkresponse(BaseModel):
    folder: str  # Folder where files are stored
    checked: Dict[str, Fileres]  # Dictionary of checked files


class Convertfullresponse(BaseModel):
    data: Convertresponse


class Checkfullresponse(BaseModel):
    data: Checkresponse


# define jsonapi compatible response like in https://jsonapi.org/
class JSONAPIResponse(JSONResponse):
    media_type = "application/vnd.api+json"

    def render(self, content: Any) -> bytes:
        return json.dumps(
            {
                "data": content,
            }
        ).encode("utf-8")


# define zip response
class ZipResponse(FileResponse):
    media_type = "application/zip"

    def __init__(self, content: bytes, filename: str, **kwargs: Any) -> None:
        super().__init__(content, **kwargs)
        content_disposition = f"attachment; filename={filename}"
        self.headers["Content-Disposition"] = content_disposition


def zipfolder(adir, zip_filename):
    filelist = []
    for dirpath, dirnames, filenames in os.walk(adir, topdown=True):
        filelist += [os.path.join(dirpath, file) for file in filenames]

    s = io.BytesIO()
    zf = zipfile.ZipFile(s, "w")

    for fpath in filelist:
        # Calculate path for file in zip
        fdir, fname = os.path.split(fpath)

        # Add file, at correct path
        zf.write(fpath, fname)

    # Must close zip for all contents to be written
    zf.close()

    # Grab ZIP file from in-memory, make response with correct MIME-type
    resp = Response(
        s.getvalue(),
        media_type="application/x-zip-compressed",
        headers={"Content-Disposition": f"attachment;filename={zip_filename}"},
    )

    return resp


# define static files
app.mount("/ui", StaticFiles(directory="ui", html=True), name="static")


def save_file(filename, data):
    with open(filename, "wb") as f:
        f.write(data)


@app.get("/", tags=["main"], status_code=200)
async def main():
    """
    Check that API is alive
    """
    res = {"message": "API is UP"}
    return JSONAPIResponse(content=res)


@app.post(
    "/checkfiles",
    response_model=Checkfullresponse,
    tags=["checkfiles"],
    status_code=201,
)
async def checkfiles(
    request: Request,
    folder: Optional[str] = None,
    files: List[UploadFile] = File(...)
):
    """
    Check SWC files for compliance with the standard"""
    if folder is None or folder == "":
        N = 10
        namearr = random.choices(string.ascii_letters + string.digits, k=N)
        foldername = "".join(namearr)
    else:
        foldername = folder
    usefolder = os.path.join("data", foldername)
    
    infolder = os.path.join(usefolder, "input")
    outfolder = os.path.join(usefolder, "output")
    logfolder = os.path.join(usefolder, "logs")
    if not os.path.exists(usefolder):
        os.mkdir(usefolder)
        if not os.path.exists(infolder):
            os.mkdir(infolder)
        if not os.path.exists(outfolder):
            os.mkdir(outfolder)
        if not os.path.exists(logfolder):
            os.mkdir(logfolder)

    fileres = {}
    swcfiles = [item for item in files
                if os.path.splitext(item.filename)[1] == ".swc"]
    nonswcfiles = [
        item for item in files if os.path.splitext(item.filename)[1] != ".swc"
    ]
    for file in swcfiles:
        contents = await file.read()
        neuronname = os.path.splitext(file.filename)[0]
        logfile = os.path.join(logfolder, neuronname + ".csv")
        save_file(os.path.join(infolder, file.filename), contents)
        (report, chkstatus, flags) = standard.chksingle(
            os.path.join(infolder, file.filename)
        )
        fileres[file.filename] = {}
        fileres[file.filename]["report"] = report.to_html()
        report.to_csv(logfile)
        fileres[file.filename]["status"] = chkstatus

    for file in nonswcfiles:
        fileres[file.filename] = {}
        fileres[file.filename]["report"] = []
        fileres[file.filename]["status"] = "FAIL"

    res = jsonable_encoder({"folder": foldername, "checked": fileres})
    return JSONAPIResponse(content=res)


@app.post(
    "/convertfiles",
    response_model=Convertfullresponse,
    tags=["convertfiles"],
    status_code=201,
)
async def convertfiles(
    request: Request,
    folder: Optional[str] = None,
    files: List[UploadFile] = File(...)
):
    """
    Convert to SWC format
    input: foldername, list of files
    output: zip file with converted files
    """

    if folder is None or folder == "":
        N = 10
        namearr = random.choices(string.ascii_letters + string.digits, k=N)
        foldername = "".join(namearr)
    else:
        foldername = folder
    usefolder = os.path.join("data", foldername)

    infolder = os.path.join(usefolder, "input")
    outfolder = os.path.join(usefolder, "output")
    logfolder = os.path.join(usefolder, "logs")
    if not os.path.exists(usefolder):
        os.mkdir(usefolder)
        if not os.path.exists(infolder):
            os.mkdir(infolder)
        if not os.path.exists(outfolder):
            os.mkdir(outfolder)
        if not os.path.exists(logfolder):
            os.mkdir(logfolder)

    fileres = {}
    for file in files:
        contents = await file.read()
        neuronname = os.path.splitext(file.filename)[0]
        outputfile = os.path.join(outfolder, neuronname + ".swc")
        logfile = os.path.join(logfolder, neuronname + ".csv")

        save_file(os.path.join(infolder, file.filename), contents)
        ending = os.path.splitext(file.filename)[1]
        fileres[file.filename] = {}
        if ending == ".swc":
            (report, chkstatus, flags) = standard.chksingle(
                os.path.join(infolder, file.filename)
            )

            creport = standard.swccorrect(
                os.path.join(infolder, file.filename),
                report,
                flags,
                outputfile,
                write_csv=False,
            )
            fileres[file.filename]["report"] = creport.to_html()
            creport.to_csv(logfile)
            fileres[file.filename]["status"] = "SUCCESS"
        else:
            (fileres[file.filename]["status"], report) = convert.single(
                os.path.join(infolder, file.filename), outputfile
            )
            if report is not None:
                fileres[file.filename]["report"] = report.to_html()
                report.to_csv(logfile)
            else:
                report_html = "<html><body>FAILED</body></html>"
                fileres[file.filename]["report"] = report_html

    res = jsonable_encoder(
        {
            "folder": foldername,
            "converted": fileres,
        }
    )
    return JSONAPIResponse(content=res)


@app.get("/getzipped/{folder}", tags=["getzipped"])
async def getzipped(request: Request, folder: str):
    """
    Get the zipped folder for a given folder"""
    folder = os.path.join("data", folder)
    outfolder = os.path.join(folder, "output")
    res = zipfolder(outfolder, folder + ".zip")
    return res


@app.get(
    "/getlogs/{folder}",
    tags=["getlogs"],
    response_class=FileResponse,
    status_code=200
)
async def getlogs(request: Request, folder: str):
    """
    Get the logs for a given folder"""
    folder = os.path.join("data", folder)
    outfolder = os.path.join(folder, "logs")
    res = zipfolder(outfolder, folder + "logs.zip")
    return res


@app.exception_handler(RequestValidationError)
async def validation_exception_handler(request: Request,
                                       exc: RequestValidationError):
    return JSONResponse(
        status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
        content=jsonable_encoder({"errors": exc.errors(), "status": "error"}),
    )
