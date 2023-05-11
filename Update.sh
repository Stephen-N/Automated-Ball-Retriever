##Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
WHITE='\033[0m'
BLUE='\033[34m'

#Function to install with apt-get
aptgetInstall () {
  VAL=$1
  echo -e "\nNow Checking $VAL"
  if sudo apt-get install $VAL -y | grep -q 'already the newest version'; then
  	echo -e "${YELLOW}$VAL had no needed changes${WHITE}"
  else
  	echo -e "${GREEN}$VAL Installation Succeeded${WHITE}\n"
  fi
}

#Function to install with apt
aptInstall () {
  VAL=$1
  echo -e "\nNow Checking $VAL"
  if sudo apt install $VAL -y | grep -q 'already the newest version'; then
  	echo -e "${YELLOW}$VAL had no needed changes${WHITE}"
  else
  	echo -e "${GREEN}$VAL Installation Succeeded${WHITE}\n"
  fi
}

#makes user give root access
echo -e "\n${BLUE}Admin permissions will be required to install updates${WHITE}"
sudo whoami | grep -E ' '

#Main system updates
echo -e "\nFirst will check for updates on this pi..."

echo "First Set of Updates"
if sudo apt update -y | grep -q 'packages can be upgraded'; then
    echo -e "${YELLOW}Apt did need to be updated${WHITE}"
    sudo apt upgrade -y
else
  echo -e "Apt Update was not needed"
fi

if sudo apt-get update -y | grep -q 'packages can be upgraded'; then
    echo -e "${YELLOW}Apt-get did need to be updated${WHITE}"
    sudo apt upgrade -y
else
  echo -e "Apt-Get Update was not needed"
fi

#Packages
echo -e "\n${BLUE}All base updates completed. Now Checking Packages...${WHITE}"

aptgetInstall gcc

aptgetInstall apache2
aptgetInstall php7.4
aptgetInstall libapache2-mod-php7.4
aptgetInstall curl
aptgetInstall php-curl
aptgetInstall php-gd
aptgetInstall php-intl
aptgetInstall php-json
aptgetInstall php-mbstring
aptgetInstall php-xml
aptgetInstall libpqxx-dev
aptgetInstall libsdl2-dev
aptgetInstall libsdl2-image-dev
aptgetInstall libsdl2-ttf-dev
aptgetInstall wiringpi

aptInstall postgresql 
aptInstall postgresql-contrib

#sed -i ':a;N;$!ba;s|DocumentRoot /var/www/html/|DocumentRoot /var/www/_PROJECT|g' /etc/apache2/sites-available/000-default.conf

echo -e "${GREEN}\nAll Updates and installs have been completed correctly!\n\n${WHITE}"