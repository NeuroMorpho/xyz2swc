from pydantic.main import BaseModel

class APIMessage(BaseModel):
    message: str

class APIRequest(BaseModel):
    data: APIMessage
