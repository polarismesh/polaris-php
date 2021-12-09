# polaris-php

Polaris is an operation centre that supports multiple programming languages, with high compatibility to different
application framework. Polaris-php is php SDK for Polaris.

[中文文档](./README_ZH.md)

## Overview

Polaris-php provide features listed as below:

* *Service instance registration, and health check*
   
   Provides API on/offline registration instance information,  with regular report to inform caller server's healthy status. 

* *Service discovery* 
 
   Provides multiple API, for users to get a full list of server instance, or get one server instance after route rule filtering and loadbalancing, which can be applied to srevice invocation soon.

* *Service circuitbreaking* 
   
   Provide API to report the invocation result, and conduct circuit breaker instance/group insolation based on collected data, eventually recover when the system allows. 

* *Service ratelimiting* 

   Provides API for applications to conduct quota check and deduction, supports rate limit  policies that are based on server level and port.

## Quick Guide

### Build Polaris-PHP

You can see this document to build polaris-php: [BuildDoc](./doc/HowToBuild.md)

### Using API

API use guide，can reference：[ApiDoc](./doc/ApiDoc.md)

### Example

API quick start guide，can reference：[QuickStart](./examples/quickstart)
