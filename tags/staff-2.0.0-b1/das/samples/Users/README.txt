This is a basic example to show how to work with DAS.

1. install database:
 cd db/<PROVIDER>
 ./install_db

2. install datasource:
  cd datasources/<PROVIDER> && make && sudo make install

3. restart axis2c

4. compile and start client (axis2c must started):
  cd client && make
  out/*/das_users_client


Where <PROVIDER> is postgresql, mysql, sqlite or service.


When using service provider you should install sqlite db and compile and install service from service/accounts also.

