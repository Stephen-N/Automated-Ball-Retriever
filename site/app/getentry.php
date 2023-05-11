<?php
    

    function getEntry() {
        // connect to database
        $db_handle = pg_connect("host=localhost dbname=capstone user=postgres password=password");

        // get row
        $table = "statistics";
        $query = "SELECT * FROM " . $table . " ORDER BY id DESC LIMIT 1";
        $result = pg_exec($db_handle, $query);

        // set variables
        while ($row = pg_fetch_array($result)) {
            $us1 = $row['us1'];
            $us2 = $row['us2'];
            $us3 = $row['us3'];
            $us4 = $row['us4'];
            $us5 = $row['us5'];
            $photo1 = $row['photo1'];
            $photo2 = $row['photo2'];
        }

        // store in local storage
        echo "<script>localStorage.setItem('us1', '$us1');</script>";
        echo "<script>localStorage.setItem('us2', '$us2');</script>";
        echo "<script>localStorage.setItem('us3', '$us3');</script>";
        echo "<script>localStorage.setItem('us4', '$us4');</script>";
        echo "<script>localStorage.setItem('us5', '$us5');</script>";
        echo "<script>localStorage.setItem('photo1', '$photo1');</script>";
        echo "<script>localStorage.setItem('photo2', '$photo2');</script>";

    }

    getentry();

?>