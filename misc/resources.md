# Literature

## BGP

### [BGPStream: A Software Framework for Live and Historical BGP Data Analysis](http://bgpstream.caida.org/bundles/caidabgpstreamwebhomepage/pubs/bgpstream.pdf)
*Chiara Orsini, Alistair King, Danilo Giordano, Vasileios Giotsas, Alberto Dainotti
Internet Measurement Conference (IMC), Nov 2016.*
 
*"We present BGPStream, an open-source software framework for the analysis of both historical and real-time Border Gateway Protocol (BGP) measurement data. Although BGP is a crucial operational component of the Internet infrastructure, and is the subject of research in the areas of Internet performance, security, topology, protocols, economics, etc., there is no efficient way of processing large amounts of distributed and/or live BGP measurement data. BGPStream fills this gap, enabling efficient investigation of events, rapid prototyping, and building complex tools and large-scale monitoring applications (e.g., detection of connectivity disruptions or BGP hijacking attacks). We discuss the goals and architecture of BGPStream. We apply the components of the framework to different scenarios, and we describe the development and deployment of complex services for global Internet monitoring that we built on top of it."*

## RTRLib

### [RTRlib: An Open-Source Library in C for RPKI-based Prefix Origin Validation](http://page.mi.fu-berlin.de/waehl/papers/whss-roslr-13.pdf)
*Matthias Wählisch, Fabian Holler, Thomas C. Schmidt, Jochen H. Schiller
Proc. of 7th USENIX Security Workshop on Cyber Security Experimentation and Test (CSET), Berkeley, CA, USA:USENIX Assoc., 2013.*

*"A major step towards secure Internet backbone routing started with the deployment of the Resource Public KeyInfrastructure (RPKI). It allows for the cryptographic strong binding of an IP prefix and autonomous systems that are legitimate to originate this prefix. A fundamental design choice of RPKI-based prefix origin validation is the avoidance of cryptographic load at BGP routers. Cryptographic verifications will be performed only by cache servers, which deliver valid AS/prefix mappings to the RPKI-enabled BGP router using the RPKI/RTR protocol.
In this paper, we give first insights into the additional system load introduced by RPKI at BGP routers. For this purpose, we design and implement a highly efficient C library of the RPKI/RTR router part and the prefix origin validation scheme. It fetches and stores validated prefix origin data from an RTR-cache and performs origin verification of prefixes as obtained from BGP updates. We measure a relatively small overhead of origin validation on commodity hardware (5% more RAM than required for full BGP table support, 0.41% load in case of ≈92,000 prefix updates per minute), which meets real world requirements of today"*

# Repositories & Articles

## [PyBGPStream Spark examples](https://github.com/CAIDA/pybgpstream-spark-examples)
## [BGP Hackathon](https://github.com/CAIDA/bgp-hackathon)
## [Use offense to inform defense. Find flaws before the bad guys do.](https://pen-testing.sans.org/resources/papers/gcih/bgp-hijinks-hijacks-incident-response-backbone-enemy-123924)
