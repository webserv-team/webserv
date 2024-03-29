import requests

# # The URL of your server
url = 'http://localhost:3035/images/salim.jpg'

# The number of requests to send
def spam(num_requests=100):
    for i in range(num_requests):
        try:
            response = requests.get(url)
            # print(f'Request {i+1} response: {response.status_code}')
        except requests.exceptions.RequestException as e:
            print(f'Request {i+1} failed: {e}')

#run the funciton in three different processes

if __name__ == '__main__':
    import multiprocessing

    # Create three processes that run the spam function
    p1 = multiprocessing.Process(target=spam)
    p2 = multiprocessing.Process(target=spam)
    p3 = multiprocessing.Process(target=spam)

    # Start the processes
    p1.start()
    p2.start()
    p3.start()

    # Wait for the processes to finish
    p1.join()
    p2.join()
    p3.join()
