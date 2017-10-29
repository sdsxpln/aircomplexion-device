<!-- Making the reader aware of the problem statement and the KPIs that need be monitored. -->
## Air monitoring , awareness of the masses :
****
Masses may not be aware of how bad the air quality really gets. Human senses can ofcourse indicate a problem with the air we breathe with symptoms but that is only when beyond the thresholds. We dont have any sense of the quantification of the air components ,only when we faint do we realise the CO / Co2 is byond any acceptable limits.

Our product here has a sense of whats the current measure of the Co2, CO, So2, No2 and being connected to the cloud / internet it can broadcast information to the general masses.

- It can in general keep the masses aware of the low quality of the atmosphere we live in
- It then becomes easy to drive environmental messages across all the sections of the society since we have our claims backed by solid historical data.

## Coincident spikes in Co2 and dependants :
****

Measuring Co2 over the time axis along with other parameters can reveal coincident changes in the graphs and hence the possibility of an underlying mathematical relation.
Our best chance is to observe the spykes  in the graphical at the excat same time in temperature and light to know if the gaseous content really changes with time.

Winter in the client's premises needs a close vigil of the movment of Co2 content in parts per million. With the rising density we would have a intuitive rise in the Co2 corresponding to the increased density of air. Client needs to track other gaseous content as well.

- Carbon monoxide
- Nitrous oxide
- Sulphur di-oxide

<!-- this section gives the problem statement and how the system works on a very high level -->
## Cloud connected solution:
****
Devices working on the field and gathering reliable timely information would only be futile if NOT connected to cloud. The solution needs to push all the data records / information to cloud.
Web applications then can tune in such database to then bring the insights to the desk.

Cloud connectivity cannot be guarenteed 100% and in such cases , fog computing approach of maintaining offline data on the device is also advised. As such when the offline data accumulates and the connection is restored the data can then be differentially appended.

Services running on the cloud are HTTP REST tuned. They continuously listen to the devices pings and supply the data as requested by the web applications. Services as these are highly available and nimble enough to deal with concurrent requests at the same time. Service to Device channel works in the polling mode. Devices can ping and request for data, but services would not be able to push notifications unless the devices are running on static IP addresses.


## Device registration and identification :
****

Services are meant for valid / authenticated devices to communicate only. The cloud endpoints should reject / filter unwanted pings from either rogue / expired devices. A device identifies itself with a unique 36 character id that is inturn hard embedded on it.

Incase of product-under-license the devices can even extract license validity / renewal information from the cloud endpoints.We can even ask the device to hibernate incase the license is suspended / revoked.

Since devices have their data tied to the UUID , it makes it possible to use the same cloud enpoints as if it were a platform. The UUID then also makes it possible to identify the data for each of the devices.


<!-- this section  is way too technical to be put out here -->
## Reading MQ135 with your RPi3
****

This sensor is handy , gets you a electrical signal that can read at definite intervals and assess the value of Co2 in the atmosphere.

advantages :
- reliability
- simplicity
- low price
- low operating power
- good sentivity

challenges :
- preheating time - stable readings only after 24Hours of heating.
- sensitivity characters vary with temp and moisture.
