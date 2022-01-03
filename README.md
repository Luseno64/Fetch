Deploy Automation program that takes a YAML configuration file as input and deploys a Linux AWS EC2 instance with two volumes and two users.


Installation
Installation of the AWS CLI and its dependencies use a range of packaging features provided by pip and setuptools. To ensure smooth installation, it's recommended to use:

pip: 9.0.2 or greater
setuptools: 36.2.0 or greater
The safest way to install the AWS CLI is to use pip in a virtualenv:

python -m pip install awscli
or, if you are not installing in a virtualenv, to install globally:

sudo python -m pip install awscli
or for your user:

python -m pip install --user awscli
If you have the aws-cli package installed and want to upgrade to the latest version, you can run:

python -m pip install --upgrade awscli
This will install the aws-cli package as well as all dependencies.

Before using the AWS CLI, you need to configure your AWS credentials. You can do this in several ways:

Configuration command
Environment variables
Shared credentials file
Config file
IAM Role
The quickest way to get started is to run the aws configure command:

$ aws configure

AWS Access Key ID: MYACCESSKEY

AWS Secret Access Key: MYSECRETKEY

Default region name [us-west-2]: us-west-2

Default output format [None]: json

To use environment variables, do the following:

$ export AWS_ACCESS_KEY_ID=<access_key>

$ export AWS_SECRET_ACCESS_KEY=<secret_key>

To use the shared credentials file, create an INI formatted file like this:

[default]

aws_access_key_id=MYACCESSKEY

aws_secret_access_key=MYSECRETKEY

[testing]

aws_access_key_id=MYACCESKEY

aws_secret_access_key=MYSECRETKEY

and place it in ~/.aws/credentials (or in %UserProfile%\.aws/credentials on Windows). If you wish to place the shared credentials file in a different location than the one specified above, you need to tell aws-cli where to find it. Do this by setting the appropriate environment variable:

$ export AWS_SHARED_CREDENTIALS_FILE=/path/to/shared_credentials_file

To use a config file, create an INI formatted file like this:

[default]

aws_access_key_id=<default access key>
  
aws_secret_access_key=<default secret key>
  
region=us-west-1

[profile testing]
  
aws_access_key_id=<testing access key>
  
aws_secret_access_key=<testing secret key>
  
region=us-west-2
  
and place it in ~/.aws/config (or in %UserProfile%\.aws\config on Windows). If you wish to place the config file in a different location than the one specified above, you need to tell the AWS CLI where to find it. Do this by setting the appropriate environment variable:

$ export AWS_CONFIG_FILE=/path/to/config_file
  
[run code]

once you have configured and pasted your public key into your yaml file run code with:

$ python3 EC2.py

  
