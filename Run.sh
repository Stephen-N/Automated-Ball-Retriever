RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
WHITE='\033[0m'
BLUE='\033[34m'


read -p "Would you like to update and install required packages? [y/n]" yn;
    case $yn in
        [Yy]* ) chmod u+x Update.sh; ./Update.sh; break;;
        [Nn]* ) echo -e "\n\n${RED}Updates will not be checked${WHITE}\n";;
        * ) echo -e "\n${RED}Please enter y/n{WHITE}"
    esac

echo Starting PostgresSQL and Apache2
sudo service postgresql start
sudo service apache2 start

#Starting the local host
sudo systemctl enable apache2
sudo systemctl enable postgresql

echo Trying to find database

#Create Database
sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD 'password';"
 
if sudo -u postgres psql -c 'create database capstone;' | grep -q 'already exists'; then
  	echo -e "${GREEN}Database was found${WHITE}"
  else
  	echo -e "\n\n${YELLOW}Database was not found, now creating new database${WHITE}\n"
    sudo -u postgres psql capstone -c "CREATE TABLE statistics (id SERIAL PRIMARY KEY, timestamp TIMESTAMP default current_timestamp, distance FLOAT, us1 FLOAT, us2 FLOAT, us3 FLOAT, us4 FLOAT, us5 FLOAT, photo1 BOOL, photo2 BOOL);"
    echo -e "${GREEN}\nDatabase was now Created!${WHITE}"
fi

#Begin program
make clean

echo -e "${BLUE}\n\nRunning Whole System now.\n${WHITE}"

make run

sleep 3

make -s clean
echo -e "\n\n"
#sudo systemctl poweroff
exit 0

make -s clean
echo -e "\n\n"
#sudo systemctl poweroff
exit 0