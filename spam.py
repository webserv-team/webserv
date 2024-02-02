# import requests

# # The URL of your server
# url = 'http://localhost:8080'

# # The number of requests to send
# num_requests = 10000

# for i in range(num_requests):
#     try:
#         response = requests.get(url)
#         print(f'Request {i+1} response: {response.status_code}')
#     except requests.exceptions.RequestException as e:
#         print(f'Request {i+1} failed: {e}')

# import requests

# url = 'http://localhost:8080'
# data = {'key1': 'value1', 'key2': 'value2'}

# response = requests.post(url, data=data)

# print(response.status_code)
# print(response.text)