#!/bin/bash

#getting the port numbers from the user as command line arguments
#grouping the argsuements in an array


function check_and_fix_port {
    PORT=$1

    if [ -z $PORT ]
    then
        echo "Port number is not provided"
        return 1
    fi

    #checking if the port number is a number or not
    if ! [[ $PORT =~ ^[0-9]+$ ]]
    then
        echo "Port number is not a number"
        return 1
    fi

    #checking if the port number is in the range of 1024 to 65535
    if [ $PORT -lt 1024 ] || [ $PORT -gt 65535 ]
    then
        echo "Port number $PORT is not in the range of 1024 to 65535"
        return 1
    fi

    #check if port number is already in use using lsof command
    PROCESS_ID=$(lsof -i :$PORT -t)
    if [ -n "$PROCESS_ID" ]
    then
        echo "Port number is already in use by process $PROCESS_ID whith the name of $(ps -p $PROCESS_ID -o comm=)"
        read -p "Do you want to kill the process? (y/n): " KILL_PROCESS
        #checking if the user entered y or n or nothing
        while [ "$KILL_PROCESS" != "y" ] && [ "$KILL_PROCESS" != "n" ] && [ -n "$KILL_PROCESS" ]
        do
            read -p "Please enter y or n: " KILL_PROCESS
        done
        if [ "$KILL_PROCESS" != "y" ]
        then
            echo "Process $PROCESS_ID is not killed"
            return 1
        fi
        kill -9 $PROCESS_ID
        echo "Process $PROCESS_ID is killed"
        return 1
    fi

    echo "this port $PORT is free to use"
}

# Usage
args=("$@")
for i in "${args[@]}"; do
    check_and_fix_port $i
done





# if [ -z $PORT ]
# then
#     echo "Port number is not provided"
#     exit 1
# fi

# #checking if the port number is a number or not
# if ! [[ $PORT =~ ^[0-9]+$ ]]
# then
#     echo "Port number is not a number"
#     exit 1
# fi

# #checking if the port number is in the range of 1024 to 65535
# if [ $PORT -lt 1024 ] || [ $PORT -gt 65535 ]
# then
#     echo "Port number is not in the range of 1024 to 65535"
#     exit 1
# fi

# #check if port number is already in use using lsof command
# PROCESS_ID=$(lsof -i :$PORT -t)
# if [ -n "$PROCESS_ID" ]
# then
#     echo "Port number is already in use by process $PROCESS_ID"
#     kill -9 $PROCESS_ID
#     echo "Process $PROCESS_ID is killed"
#     exit 1
# fi


# echo "this port is free to use"

