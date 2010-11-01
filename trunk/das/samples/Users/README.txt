1. install database:
 cd db/<PROVIDER>
 ./install_db

2. install datasource:
  cd datasources/<PROVIDER> && make && sudo make install

3. restart axis2c

4. compile and start client (axis2c must started):
  cd client && make
  out/*/das_users_client


Where <PROVIDER> is postgresql, mysql or sqlite.

