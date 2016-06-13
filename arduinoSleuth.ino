
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" 
#include "credentials.h"
#include <Process.h>

Process date;
int sensorPin = A0;

void setup() {
  Serial.begin(9600);
  delay(4000); //wait for Serial connection to be established
Bridge.begin();
}

void loop()
{
  int lightLevel = analogRead(sensorPin); //get sensor value
  Serial.println(lightLevel); //for debugging
  
  if(lightLevel > 100){ //check the snesed value is above the darkness range
   Serial.println("Door opened! Posting to Twitter"); //notify Serial that door has been opened
  
   notifyOwner(1, getTime()); //getTime and then notifyOwner 
   
   
   while(lightLevel>100){ //keep checking the door is still open
     lightLevel = analogRead(sensorPin); //and update the sensed value until the door closes
   }
   
   Serial.println("Door closed! Posting to Twitter.."); //notify Serial that door has closed
  notifyOwner(0, getTime());  //getTime and then notifyOwner 
  }

    delay(500); //delay the loop half a second 
  }


String getTime(){
//
if (!date.running()) {
    date.begin("date");
    date.addParameter("+%T");
    date.run();
}
   return date.readString(); //return the obtained time string
}
  
  
  void notifyOwner(int i, String time){ //notify the owner of the door status and the current timestamp
    String tweetText;
    
    if(i==1){
     tweetText = String("Someone opened your vault at " + time+"!");}
    
    else if (i==0){
      tweetText = "Your vault door has been closed at " + time+".";
    }

    
    TembooChoreo StatusesUpdateChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    StatusesUpdateChoreo.begin();
    
    // set Temboo account credentials
    StatusesUpdateChoreo.setAccountName(TEMBOO_ACCOUNT);
    StatusesUpdateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    StatusesUpdateChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Twitter > Tweets > StatusesUpdate)
    StatusesUpdateChoreo.setChoreo("/Library/Twitter/Tweets/StatusesUpdate");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Tweets/StatusesUpdate/ 
    // for complete details about the inputs for this Choreo
 
    // add the Twitter account information
    StatusesUpdateChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    StatusesUpdateChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    StatusesUpdateChoreo.addInput("ConsumerKey", TWITTER_CONSUMER_KEY);    
    StatusesUpdateChoreo.addInput("ConsumerSecret", TWITTER_CONSUMER_SECRET);

    // and the tweet we want to send
    StatusesUpdateChoreo.addInput("StatusUpdate", tweetText);

    // tell the Process to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = StatusesUpdateChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
        Serial.println("Success! Tweet sent!");
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (StatusesUpdateChoreo.available()) {
        char c = StatusesUpdateChoreo.read();
        Serial.print(c);
      }
    } 
    StatusesUpdateChoreo.close();
  }


