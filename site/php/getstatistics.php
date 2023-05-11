<!DOCTYPE html>
<html>

<head>
    <title>Service Ball Retriever</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.0.0/dist/css/bootstrap.min.css" integrity="sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm" crossorigin="anonymous">

    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">

    <style>
        body {
            margin: 0px;
            width: 100vw;
            height: 100vh;
            overflow: auto;
            background-color: antiquewhite;
        }

        .block {
            background-color: #6CC;
            padding: 20px;

        }

        .top-buffer {
            margin-top: 10px;
        }

        .resize {
            max-width: 100%;
            height: auto;
        }

        table {
            width: 100%;
            border-collapse: collapse;
        }

        table,
        td,
        th {
            border: 1px solid black;
            padding: 5px;
        }

        th {
            text-align: left;
        }
    </style>

    <script>
        setTimeout(function() {
            window.location.reload(1);
        }, 400);

        function refresh() {
            window.location.reload();
        }
    </script>
</head>

<body>
    <nav class="navbar navbar-light bg-light">
        <span class="navbar-brand mb-0 h1">
            <a class="navbar-brand" style="color:blue" href="../index.html">Home</a>
            Service Ball Retriever
        </span>
    </nav>

    <?php
    /* DIAGNOSTIC INFO */
    echo "<h3>Establishing Connection</h3>";
    $db_handle = pg_connect("host=localhost dbname=capstone user=postgres password=password");

    if ($db_handle) {
        echo "Connection attempt succeeded";
    } else {
        echo "Connection attempt failed";
        die('Could not connect: ' . pg_errormessage($db_handle));
    }

    echo "<h3>Connection Information</h3>";

    echo "DATABASE NAME: " . pg_dbname($db_handle) . "<br>";
    echo "HOSTNAME: " . pg_host($db_handle) . "<br>";
    echo "PORT: " . pg_port($db_handle) . "<br>";

    echo "<h3>Checking the query status</h3>";
    $query = "SELECT * from statistics LIMIT 1";
    // $query = "INSERT INTO statistics(distance, moving, ir, magnet1, magnet2, photo1, photo2)
    // VALUES (0,false,1,false,true,true,false)";
    $result = pg_exec($db_handle, $query);
    if ($result) {
        echo "The query executed successfully. <br>";
    } else {
        echo "The query failed with the following error: <br>";
        echo pg_errormessage($db_handle);
    }
    pg_close($db_handle)
    ?>

    <!-- RECEIVING SHOW OPTIONS INPUT AND DISPLAYING REQUESTED OPTION -->
    <?php
    $table = "statistics";

    echo "<h3>Live Data</h3>";

    $con = pg_connect("host=localhost dbname=capstone user=postgres password=password");

    $query = "SELECT * FROM " . $table . " ORDER BY id DESC LIMIT 10";
    $result = pg_exec($con, $query);

    echo "<div id='#sql_data'><table style='background-color: white; overflow-x: scroll; width: 100%;'>
        <tr>
            <th>id</th>
            <th>timestamp</th>
            <th>distance</th>
            <th>us1</th>
            <th>us2</th>
            <th>us3</th>
            <th>us4</th>
            <th>us5</th>
            <th>photo1</th>
            <th>photo2</th>
        </tr></div>";
    while ($row = pg_fetch_array($result)) {
        echo "<tr>";
        echo "<td>" . $row['id'] . "</td>";
        echo "<td>" . $row['timestamp'] . "</td>";
        echo "<td>" . $row['distance'] . "</td>";
        echo "<td>" . $row['us1'] . "</td>";
        echo "<td>" . $row['us2'] . "</td>";
        echo "<td>" . $row['us3'] . "</td>";
        echo "<td>" . $row['us4'] . "</td>";
        echo "<td>" . $row['us5'] . "</td>";
        echo "<td>" . $row['photo1'] . "</td>";
        echo "<td>" . $row['photo2'] . "</td>";
        echo "</tr>";
    }
    echo "</table>";

    // echo "<div id='#sql_data'><table style='background-color: white; overflow-x: scroll; width: 100%;'>
    //     <tr>
    //         <th>id</th>
    //         <th>timestamp</th>
    //         <th>distance</th>
    //         <th>moving</th>
    //         <th>ir</th>
    //         <th>magnet1</th>
    //         <th>magnet2</th>
    //         <th>photo1</th>
    //         <th>photo2</th>
    //     </tr></div>";
    // while ($row = pg_fetch_array($result)) {
    //     echo "<tr>";
    //     echo "<td>" . $row['id'] . "</td>";
    //     echo "<td>" . $row['timestamp'] . "</td>";
    //     echo "<td>" . $row['distance'] . "</td>";
    //     echo "<td>" . $row['moving'] . "</td>";
    //     echo "<td>" . $row['ir'] . "</td>";
    //     echo "<td>" . $row['magnet1'] . "</td>";
    //     echo "<td>" . $row['magnet2'] . "</td>";
    //     echo "<td>" . $row['photo1'] . "</td>";
    //     echo "<td>" . $row['photo2'] . "</td>";
    //     echo "</tr>";
    // }
    // echo "</table>";

    pg_close($con);
    ?>

    <div class="container">
        <div class="row top-buffer">
            <div class="col text-center">
                <button type="button" class="block btn btn-lg btn-block" id='refresh' onclick="refresh()" style="color: white;">Refresh Data</button>
            </div>
        </div>
    </div>

</body>

</html>