<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Welcome User</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            margin: 0;
            padding: 0;
            display: flex;
            align-items: center;
            justify-content: center;
            height: 100vh;
        }

        .welcome-container {
            text-align: center;
            padding: 20px;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }

        h1 {
            color: #333;
        }
    </style>
</head>
<body>

<?php
// Check if the username is provided in the POST data
    
    if (isset($_POST['username']))
    {
        $password = $_POST['password'];
        $password = htmlspecialchars($password, ENT_QUOTES, 'UTF-8');
        if ($password == 'password')
        {
            $username = htmlspecialchars($_POST['username'], ENT_QUOTES, 'UTF-8');
            echo '<div class="welcome-container">';
            echo '<h1>Welcome, ' . $username . '!</h1>';
            echo '</div>';
        }
        else
        {
            echo '<div class="welcome-container">';
            echo '<h1>Wrong password!</h1>';
            echo '</div>';
        }

    }
    else
    {
        echo '<div class="welcome-container">';
        echo '<h1>Welcome, user!</h1>';
        echo '</div>';
    }
?>

</body>
</html>
