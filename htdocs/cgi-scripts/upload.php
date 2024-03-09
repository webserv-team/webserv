<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Assuming you are expecting a file input named 'file'
    if (isset($_FILES['file'])) {
        $uploadDirectory = 'upload/';
        $uploadedFileName = $_FILES['file']['name'];
        $targetPath = $uploadDirectory . $uploadedFileName;

        // Move the uploaded file to the desired directory
        if (move_uploaded_file($_FILES['file']['tmp_name'], $targetPath)) {
            echo "File uploaded successfully. Saved as: " . $targetPath;
        } else {
            echo "Error uploading file.";
        }
    } else {
        echo "No file uploaded.";
    }
} else {
    echo "Invalid request method.";
}
?>
