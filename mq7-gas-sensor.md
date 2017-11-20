### Carbon Monoxide, significance of detecting it :
****

Carbon monoxide (CO) is a very __dangerous gas which is odorless, colorless, and tasteless__, so it cannot be smelt, seen, or tasted. A person really would have no idea that they are breathing in CO besides the fact that they would start to feel horrible. The most common __symptoms of CO poisoning is headache, nausea, vomiting, dizziness, fatigue, and a feeling of weakness. Neurological signs include confusion, disorientation, visual disturbance, syncope, and seizures.__
Carbon monoxide is produced from the __partial oxidation of carbon-containing compounds__; it forms when there is not enough oxygen to produce carbon dioxide (CO), such as when operating a stove or an internal combustion engine in an enclosed space. So it is really in enclosed spaces with __partial oxidation__ of carbon products that creates the danger of carbon monoxide production in homes or in business environments.
Carbon monoxide poisoning is the most common type of fatal air poisoning in many countries. Being colorless, odorless, and tasteless, it is very hard to detect but __highly toxic.__ Carbon monoxide is absorbed through breathing and enters the bloodstream through gas exchange in the lungs. CO combines with hemoglobin to produce carboxyhemoglobin, which __usurps the space in hemoglobin that normally carries oxygen__, rendering blood inffective to carry and supply oxygen to bodily tissues. This leads to oxygen deprivation, which can be deadly.

![CO poisoning](carbon-monoxide-poisoning-1000x250.jpg)

>> This gas is as nasty  as it can get. Being virtually transperant to majority of the human sense organs, it manifests itself only by symptoms bordering fatality.

__CO is measured in parts per million (ppm).__ To give you some perspective, the natural atmosphere is composed of 0.1ppm. In there average level in homes is 0.5-5ppm. The level near properly adjusted gas stoves in homes and from modern vehicle exhaust emissions is 5-15ppm. The exhaust from automobilies in Mexico City central area is 100-200ppm.

![CO kills](download.png)

Having this overview of carbon monoxide gives some background to how it is created and the real severe dangers it can pose. It creates context to just how important it is to be able to detect and measure the amount of CO that may be present in the environment during any given time.

[ Excerpt from here ](http://www.learningaboutelectronics.com/Articles/MQ-7-carbon-monoxide-sensor-circuit-with-arduino.php)

### MQ7 , flying fish sensor module :
****

![Sensor module commercially available](MQ-7-MODULE-CARBON-MONOXIDE-GAS-SENSOR.png)

This module is something you IoT enthusiasts would have come across on most of the e-commerce sites. If you have had any experiences with other gas sensor modules like MQ135, this might come to you as a surpirse. Requirement of of having to heat the sensor with differential power rating is one major change.

The module though is well laid out and the sensor if operated correctly as in the datasheet guidelines , is capable of yeilding pretty stable and reliable values. What is then a bit twisted is the way the ppm reading derived from the voltage readings keeping the log-log characteristics in reference. Here I attempt to explain the method of arriving at the CO concetration. This may help you to design your algorithm running on Pi.

The module has the following significant components mounted

1. __Sensor__ : Is the heart of the module. It has a inbuild heater coil and also the Sno2 filament that accumulates the CO deposition to eventually show characteristic change in the electrical resistance. The heater and the filament are non-divisible, in the sense they cannot be separated any further from the packing.
2. __Load resistance__ : This a surface mounted component along with the module. Unless you are getting a non-modular sensor [Like it is offered here](https://www.pololu.com/product/1482) you have a load resistance already in the module. Pick up a multimter and measure it between A0 and GND. - I have observed this to be 1KOhm in my case.
3. __LEDs for threshold__ indication: There are times when you are looking for the digital signal from the sensor only when a certain threshold is crossed against set threshold levels.
4. sensitivity adjustment __potentiometer__:This lets you adjust the threshold at which the digital signal should go high.

### Schema of things and getting this connected to your RPi
****
The RPi 3B has no GPIOs that can take in analog in. Reading the voltage then has a direct limitation with RPi. We use a ADS115 [see specifications here](http://www.ti.com/lit/ds/sbas444c/sbas444c.pdf) to then connect over I2C and read the voltages stored onto the ADS resgisters. 
Instead of the ADS115 you can also opt for :
1. MPC3208 + SPI
2. can build your own RC timer - nerdy but still effective for the voltages we are reading.

We would be using the A0 of the sensor , connected to the ADS (any channel from the available 4) and the ADS then inturn connected to RPi.

### MQ7 characteristics and first glance through the datasheet
***

### Differential heating requirements and priming
****

### Arriving at the sensor resistance from output voltage:
****

### Calculating ppm CO from sensor resistance
****
