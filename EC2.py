from os import read
from collections import defaultdict
import boto3
import yaml

KEYNAME="mykeypair2"
IMAGES = defaultdict(str)
IMAGES ['amzn2-hvm-x86_64'] = 'ami-00f7e5c52c0f43726'

client =  boto3.client('ec2')
resource = boto3.resource('ec2')

#reads yaml file.returns data.
def read_yml():
      with open('aws.yml') as f:
         data = yaml.load(f,Loader= yaml.FullLoader)
         return data

data = read_yml()
data = data['server']
vol1 = data['volumes'][0]
vol2 = data['volumes'][1]
user1 = data['users'][0]
user2 = data['users'][1]
   
#creates key pairs : private and public key pair for aws
def create_key_pair():
   with open(KEYNAME+'.pem', 'w') as kpfile:
        try:
            key_pair = resource.create_key_pair(KeyName=KEYNAME)
            kpfile.write(str(key_pair.key_material))
            
        except Exception as e:
            print('Coud not create key-pair.')
            print(type(e).__name__)
      
#create AMI 
def create_ami():
    ami = data['ami_type']
    ami += '-' + data['virtualization_type']
    ami += '-' +data['architecture']
    return ami

#creates user file - specifically file systems as well as access to.
def get_user_data():
    user_data = '#!/bin/bash\n'
    # mount vol2
    user_data += 'mkfs.%s %s\n'        % (vol2['type'], vol2['device'])
    user_data += 'mkdir %s\n'          % (vol2['mount'])
    user_data += 'mount -o rw %s %s\n' % (vol2['device'], vol2['mount'])
    user_data += '7chmod 077 %s\n'          % (vol2['mount'])
    # create user1
    user_data += 'adduser %s\n'        % (user1['login'])
    user_data += 'mkdir /home/%s/.ssh\n' % (user1['login'])
    user_data += 'touch /home/%s/.ssh/authorized_keys\n' % (user1['login'])

    user_data += 'echo %s > /home/%s/.ssh/authorized_keys\n' % (user1['ssh_key'], user1['login'])
    #mount vol1
    user_data += 'mkfs.%s %s\n'        % (vol1['type'], vol1['device'])
    user_data += 'mkdir %s\n'          % (vol1['mount'])
    user_data += 'mount -o rw %s %s\n' % (vol1['device'], vol1['mount'])
    user_data += 'chmod 0777 %s\n'          % (vol1['mount'])
    # create user2
    user_data += 'adduser %s\n'        % (user2['login'])
    user_data += 'mkdir /home/%s/.ssh\n' % (user2['login'])
    user_data += 'touch /home/%s/.ssh/authorized_keys\n' % (user2['login'])
    user_data += 'echo %s > /home/%s/.ssh/authorized_keys\n' % (user2['ssh_key'], user2['login'])
    return user_data

# ssh access creation.
def ssh_access(instance):
        describe = client.describe_instances(
           InstanceIds=[
             instance[0].id
         ]
        )
        sgId = describe['Reservations'][-1]['Instances'][0]['SecurityGroups'][0]['GroupId']

        try: 
         resp = client.authorize_security_group_ingress(
               GroupId=sgId,
               IpPermissions=[
                  {'IpProtocol': 'tcp',
                  'FromPort': 22,
                  'ToPort': 22,
                  'IpRanges': [{'CidrIp': '0.0.0.0/0'}]}
         ])

         return resp
        except Exception as e:
           print(e)

def main():
       create_key_pair()
       user_data = get_user_data()
       print('    ----getuserdata-----    ')
       print(user_data)  
       ami = create_ami()
       instance = resource.create_instances(
        KeyName=KEYNAME,
        ImageId=IMAGES[ami],
        InstanceType=data['instance_type'],
        MinCount=data['min_count'],
        MaxCount=data['max_count'],
        UserData=user_data,
        BlockDeviceMappings=[
            {
                'DeviceName': vol1['device'],
                'Ebs': {
                    'VolumeSize': vol1['size_gb']
                }
            },
            {
                'DeviceName': vol2['device'],
                'Ebs': {
                    'VolumeSize': vol2['size_gb']
                }
            }
        ]
    ) 
       print('-----instances-----')
       print(instance)
       ssh_access(instance)
main()


