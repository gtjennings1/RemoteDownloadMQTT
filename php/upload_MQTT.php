/******************************************************************************
Copyright 2018 Gnarly Grey LLC
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/
<?php
 if (isset($_POST['upload'])) {
     //here the file selected is being saved into a variable
     $file = file_get_contents($_FILES['inputfile']['tmp_name']);
     echo "Length of file is: " . strlen($file) . PHP_EOL;
    //  publish_message($file, 'arduino', '52.91.113.142', 1883, 20);
     // publish_message($file, 'arduino', 'm11.cloudmqtt.com', 1883, 20);
     // publish_message($file, 'arduino', 'm11.cloudmqtt.com', 9331, 20);
     publish_message($file, 'arduino', 'm11.cloudmqtt.com', 10331, 20);
 }


//

function publish_message($msg, $topic, $server, $port, $keepalive) {

    $client = new Mosquitto\Client();
    $client->onConnect(function($r) use ($client, $msg, $topic){
          print_connection_status($r);
          $mid = $client->publish($topic, $msg);
          echo " ...           Submitted message for publishing with mid=$mid\n";
    });
    $client->onDisconnect('disconnect');
    $client->onPublish(function($p) use ($client) {

        echo " >>> PUBLISH   Mesage published, code ($p)\n";

        // maybe here we actually disconnect?
        $client->disconnect();
    });

    $client->onLog(function($log){
      // echo " >>> LOG   $log\n";
    });

    $client->setCredentials('puquorpi', 'oigDBH4EMT8S');
    $client->connect($server, $port, $keepalive);

    echo " ----------------------- \n";
    $client->loopForever();

    // throws 'The client is not currently connected.' exception if port where connection should happen is closed
    //   -----------------------
    //  PHP Fatal error:  Uncaught Mosquitto\Exception: The client is not currently connected. in /home/mars/w/grant-website/upload-cli3.php:45
    //  Stack trace:
    //  #0 /home/mars/w/grant-website/upload-cli3.php(45): Mosquitto\Client->loopForever()
    //  #1 /home/mars/w/grant-website/upload-cli3.php(15): publish_message('ff 00\r\n4c 61 74...', 'arduino', 'm11.cloudmqtt.c...', 1883, 20)
    //  #2 {main}

}

/*****************************************************************
 * Call back functions for MQTT library
 * ***************************************************************/
function print_connection_status($r) {
    if($r == 0) echo " >>> CONNECT   {$r}-Connection successsful|\n";
    if($r == 1) echo " >>> CONNECT   {$r}-Connection refused (unacceptable protocol version)|\n";
    if($r == 2) echo " >>> CONNECT   {$r}-Connection refused (identifier rejected)|\n";
    if($r == 3) echo " >>> CONNECT   {$r}-Connection refused (broker unavailable )|\n";
}


function disconnect($c) {
    echo " >>> DISCONNECT    Disconnected code ($c)\n\n";
}

?>

<form action="#" method="POST" enctype="multipart/form-data">
    <br>
    <input type="file" name="inputfile" id="file" />
    <input class="button" style="margin-top:2em;" name="upload" type="submit" value=" Upload & Send">
</form>