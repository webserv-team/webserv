chrome.webRequest.onBeforeRequest.addListener(
    function(details) {
      console.log("Request URL:", details.url);
      console.log("Request Method:", details.method);
      console.log("Request Type:", details.type);
      console.log("Request TimeStamp:", details.timeStamp);
      console.log("Request Headers:", details.requestHeaders);
  
      // You can also log other information as needed
  
      // Returning {cancel: true} will block the request
      // return { cancel: true };
    },
    { urls: ["<all_urls>"] },
    ["requestHeaders"]
  );
  