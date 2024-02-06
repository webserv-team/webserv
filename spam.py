import requests

# The URL of your server
url = 'http://localhost:8080'

# The number of requests to send
num_requests = 10000

for i in range(num_requests):
    try:
        response = requests.get(url)
        print(f'Request {i+1} response: {response.status_code}')
    except requests.exceptions.RequestException as e:
        print(f'Request {i+1} failed: {e}')