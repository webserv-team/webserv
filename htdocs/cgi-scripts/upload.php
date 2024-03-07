<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Assuming you are expecting a file input named 'file'
    if (isset($_FILES['file'])) {
        $fileContent = file_get_contents($_FILES['file']['tmp_name']);

        // Output the content
        echo $fileContent;

        // You might want to do further processing or save the content to a file, etc.
    } else {
        echo "No file uploaded.";
    }
} else {
    echo "Invalid request method.";
}
?>
