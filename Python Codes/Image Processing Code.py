import numpy
import cv2
import serial
########################################
##Read Image and Converting into GrayScale
########################################
img=cv2.imread('Puzzle Solver 2 Bonus.jpg')
 
gray=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
ret,thresh=cv2.threshold(gray,127,255,0)
contours,hierarchy=cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

#########################################
### Function Containing Parameters for identifing Elements of Matrix
#########################################
def decode(x):
    A=cv2.contourArea(contours[x])
    P=cv2.arcLength(contours[x],True)
    if A>=1220 and A<=1290 and P>=230 and P<=240:
        return 2 
    elif A>=800 and A<=900:
        return 1
    elif A>980 and A<=1020:
        return 7    
    elif A>1200 and A<=1240:
        return 3
    elif A>=1240 and A<=1280:
        return 5
    elif A>=1350 and A<=1420:
        return 4
    elif A>=1800 and A<=1840:
        return 8
    elif A>=1850 and A<=1890:
        return 0
    elif A>=1480 and A<=1620:
        return 9
    elif A>=1615 and A<=1650: 
        return 6
                
##########################################
### Drawing Contours on the elements
#########################################    

list1=[]
len1=len(contours)
for i in range(0,len1):
   Area1 =cv2.contourArea(contours[i])
   if Area1>10000 and Area1<13000:
       list1.append('-')
   if Area1>=800 and Area1<=2500:
       list1.append(i)
       cv2.drawContours(img,contours,i,(0,255,0),2)
list1.reverse()       

###########################################
### Making a list of elements of Matrix1 using list1
############################################
len2=len(list1)     
D1=[]
D2=[]
count=0
len3=0
for j in range(0,len2):
    if count%4==0 and count!=0 and len3<=18:
        D1.extend(list1[j:j+6])
        len3=len(D1)
        count=-3
    if list1[j]=='-':
        count=count+1                     
###########################################
###### Making a list of elements of Matrix2 using list1
##########################################        

count=0
D2=list1[:]
len4=len(D2)
D2length=0
for j in D2:
    if count%4==0 and count!=0 and D2length<=18:
        ind=D2.index(j)
        for k in range(0,6):
            D2[ind+k]='*'
        count=0      
    if j=='-':
        count=count+1
        D2length=D2length+1                        
############################################
##### Converting of '-' into the cell numbers(index)
############################################        
list2=[]
for j in D2:
    if j!='*':
        list2.append(j)


for j in range(0,12):
    D1.remove('-')
    

list3=[]
count=0
for j in range(0,len(list2)):
    if list2[j]=='-':
        count=count+1
    if list2[j]=='-' and type(list2[j-1])==type(1):
        if type(list2[j-2])==int:
            list3.append([count-1,list2[j-2],list2[j-1]])
        else:
            list3.append([count-1,list2[j-1]])    
                                          
############################################
#### Decoding elements of D1 and list3 using function decode
############################################            
list4=[]
for j in D1:
    list4.append(decode(j))


list5=[]
for j in range(0,len(list3)):
        if len(list3[j])==2:
            list5.append([list3[j][0],decode(list3[j][1])])
        elif len(list3[j])==3:
            list5.append([list3[j][0],decode(list3[j][1]),decode(list3[j][2])])


list6=[]
for j in range(0,len(list5)):
        if len(list5[j])==2:
            list6.append(list5[j])
        elif len(list5[j])==3 and list5[j][1]==0:
            list6.append([list5[j][0],int(str(list5[j][2])+str(list5[j][1]))])
        else:
            list6.append([list5[j][0],int(str(list5[j][1])+str(list5[j][2]))])
print 'D1 =',list4   ## Main Matrix 1 list

for m in list6:
    if m[1]>20:
        str00=str(m[1])
        str11=str00[1]+str00[0]
        m[1]=int(str11)

print 'D2 =',list6   ## Main Matrix 2 list
print list6
      
               
                                                
########################################             
cv2.imshow('image',img)
cv2.waitKey(0)
cv2.destroyAllWindows()
#########################################

def Sum_of_number(Givenarray,Required_number):
    c=Givenarray[:]
##############################################
## Helper1 Function
##############################################
## This function halves the required number and then returns half-1 and half+1
#############################################    
    def helper1(Required_number):
        half=float(Required_number)/2
        if half==int(half):
            h1=int(half)-1
            h2=int(half)+1
            return [h1,h2,half]
        else:
            h1=int(half)
            h2=int(half)+1
            return [h1,h2,1.5]
###############################################
## This is helper2 recursive function
###############################################        
## This function sees that if half-1 and half+1 are in the given required list
## If not then it increment or decrement the number and then finds the required number that exists
###############################################        
    
    def helper2(Givenarray,h1,h2,half):
        if half==int(half) and half<10:
            if Givenarray.count(int(half))>=2:
                Givenarray.remove(int(half))
                Givenarray.remove(int(half))
                return Givenarray
        if (h1 in Givenarray) and (h2 in Givenarray):
            Givenarray.remove(h1)
            Givenarray.remove(h2)
            return Givenarray
        else:    
            flag=0
            a=h1
            b=h2
            while(flag==0):
                a=a-1
                b=b+1
                if (a in Givenarray) and (b in Givenarray): 
                     Givenarray.remove(a)
                     Givenarray.remove(b)
                     return Givenarray
                if a<0:
                    flag=1     
        if flag==1:               
            if (h1 in Givenarray) and not(h2 in Givenarray):
                Givenarray.remove(h1)
                return helper2(Givenarray,helper1(h2)[0],helper1(h2)[1],helper1(h2)[2])
            elif not(h1 in Givenarray) and (h2 in Givenarray):
                Givenarray.remove(h2)
                return helper2(Givenarray,helper1(h1)[0],helper1(h1)[1],helper1(h1)[2])
            else:
                return helper2(Givenarray,h1-1,h2+1,1.5)    
    
    if Required_number in Givenarray:
        return [Required_number]
   
    else:                            
        a=helper1(Required_number)              ### Calling of helper1
        Ans=helper2(Givenarray,a[0],a[1],a[2])  ### Calling of helper2 
        for i in Ans:
            if i in c:
                c.remove(i) 
        return c
        
def puzzle(d1,d2):

    output1=[]     
    for m in range(0,len(d2)):   
        number1 = Sum_of_number(d1,d2[m][1])
        z=str(d2[m][1])+"="+str(number1[0])
        for j in range(1,len(number1)):
            y=str(number1[j])
            z=z+"+"+y
        print(z)
            
        number1.append(d2[m][1])
        output1.extend(number1)

    return output1

puzzle(list4,list6)
        



