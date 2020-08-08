var Connection=require('tedious').Connection;
var Request = require('tedious').Request;
var TYPES=require('tedious').TYPES;

module.exports = function (context, IoTHubMessage) {
    //context.log(`JavaScript eventhub trigger function called for message array: ${IoTHubMessage}`);
    IoTHubMessage.forEach(message => {
        console.log(`Processed message: ${message}`);
        console.log(JSON.stringify(message));
    });
    var _currentData={}
    var res={}
    var config={
        userName: 'maxwell-db_admin',
        password: 'humanitytech2020!',
        server:   'maxwell-db.database.windows.net',
        options:  {
            encrypt: true,
            database: 'maxwell-database'
        }
    };
    /*context.log(JSON.stringify(config))*/
    IoTHubMessage.forEach((message ,index) => {
        console.log(message.device_id);
        console.log(message.occupancy);
        console.log(message.usages);
        console.log(message.inevents);
        console.log(message.device_id);
        console.log(message.incleanevents);
        console.log(context.bindingData.enqueuedTimeUtcArray[index]);
    });

    var connection=new Connection(config);
    connection.on('connect',function(err){

        console.log("connected");


        //use the connection as normal
        IoTHubMessage.forEach((message ,index) => {
            var request = new Request(
                "INSERT INTO dbo.main_device_update" +
                "   (device_id,creation_date,occupancy,usages,inevents,incleanevents)" +
                "VALUES" +
                "   (@device_id,@creation_date,@occupancy,@usages,@inevents,@incleanevents);", 
                function (err) {
                    if (err)
                        console.error(err);
            });
            var date= new Date(context.bindingData.enqueuedTimeUtcArray[index]);
            request.on('requestCompleted', function(err) { console.log("requestCompleted");});
            request.addParameter('device_id', TYPES.VarChar,message.device_id);
            request.addParameter('creation_date', TYPES.DateTime,date);
            request.addParameter('occupancy', TYPES.Int,message.occupancy);
            request.addParameter('usages', TYPES.Int, message.usages);
            request.addParameter('inevents', TYPES.Int,message.inevents);
            request.addParameter('incleanevents', TYPES.Int, message.incleanevents);
            connection.execSql(request);
        });
    });
    context.done();
};


