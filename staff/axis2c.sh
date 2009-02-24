
if [ -z "$AXIS2C_HOME" ]
then
  AXIS2C_HOME='/usr/local/axis2c'
  PATH="$PATH:${AXIS2C_HOME}/bin"
  LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${AXIS2C_HOME}/lib"
fi

export AXIS2C_HOME
export PATH
export LD_LIBRARY_PATH
