# Server Properties
This serves as documentation for the various different properties that can be used to configure the server in the server.properties file.

## webserver-port
The port that the webserver will attempt to bind to

## find-next-available-port
Specify whether you would like the server to attempt to bind to the next available port if the default port is not available

## webserver-connection-queue-size
The size of the queue of pending connections

## webserver-use-ipv6
Use IPv6?  Not currently supported

## threadpool-size
The size of the threadpool that services incoming connections

## threadpool-busy-wait-timer
The time in seconds to idle in between servicing incoming connections, helps reduce CPU load

## request-mapping-domain
The size of potential mapping locations for bindings, larger values may increase performance at the cost of memory

## webserver-root-dir
The root directory of static content that the webserver serves

## webserver-index
The static index file path relative to webserver-root-dir
