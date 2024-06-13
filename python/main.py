import serial
import requests

arduino = serial.Serial(port='COM5', baudrate=9600, timeout=.1)

def read_serial():
    x = arduino.readline()
    string = x.decode('utf-8').strip()
    if string: print(string)
    data = string.strip().split('|')
    if data[0] == 'data':
        print('player: ' + data[1] + ' score: ' + data[2])
        return data

def authenticate() -> str:
    print('Authenticating...')
    result = requests.post('https://hackaton-bun-sercer.onrender.com/login',data={'username':'admin', 'password':'r&9N#w*R9eDi5h'})
    jwt = result.cookies.get('auth')
    if not jwt: raise Exception('Failed to authenticate')
    print('Authenticated')
    return jwt

def send_data(data: list[str], jwt: str) -> bool:
    result = requests.post('https://hackaton-bun-sercer.onrender.com/scores',data={'player': data[1], 'score': data[2]},cookies={'auth':jwt})
    print(result.status_code, result.text)
    return result.ok

if __name__ == '__main__':
    jwt = authenticate()
    while True:
        data = read_serial()
        if not data: continue
        isOk = send_data(data, jwt)
        if isOk: arduino.write('ok'.encode('utf-8'))
        else: arduino.write('fail'.encode('utf-8'))

