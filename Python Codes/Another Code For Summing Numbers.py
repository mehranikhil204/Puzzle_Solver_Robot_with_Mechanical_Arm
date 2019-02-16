import cv2
import numpy

#Teams can add other helper functions
#which can be added here
def SumofTwo(a,b):
    half=b/2
    flag=0
    n=0
    if int(b)/2==10:
        return [a,-1,-1]
    elif half==int(b)/2:
        if a.count(int(b)/2)>=2:
            flag=1
            for i in range(0,2):
                a.remove(int(b)/2)
            return [a,int(b)/2,int(b)/2]
    if flag==0:
        while(flag==0):
            if half==int(half):
                h1=int(half)-n-1
                h2=int(half)+n+1
            else:
                h1=int(half)-n
                h2=int(half)+n+1    
            if h2>=10:
                return [a,-1,-1]
            else:
                if (h1 in a) and (h2 in a):
                    flag=1
                    a.remove(h1)
                    a.remove(h2)
                    return [a,h1,h2]
            n=n+1        

                        
def SumofThree(a,b):
    d1=b/2
    if d1==int(b)/2:
        h1=int(d1)-1
        h2=int(b)/2+1
    else:
        h1=int(d1)
        h2=int(d1)+1
    flag=0            
    while(flag==0):
        if h1 in a:
            c=a
            c.remove(h1)
            h3=SumofTwo(c,float(h2))
            if h3[1]==-1:
                c.append(h1)
                h1=h1-1
                h2=h2+1
            else:
                return [h3[0],h1,h3[1],h3[2]]  
        h1=h1-1
        h2=h2+1
        if h1<=-1:
            return [a,-1,-1,-1]
            
def SumofFour(a,b):
    d1=b/2
    if d1==int(b)/2:
        h1=int(d1)-1
        h2=int(b)/2+1
    else:
        h1=int(d1)
        h2=int(d1)+1
    flag=0            
    while(flag==0):
        h1result=SumofTwo(a,float(h1))
        h2result=SumofTwo(a,float(h2))
        if h1result[1]==-1 or h2result[1]==-1:
            h1=h1-1
            h2=h2+1
        else:
            flag=1
            return [h2result[0],h1result[1],h1result[2],h2result[1],h2result[2]]  
        if h1==-1:
            return [a,-1,-1,-1,-1]                  
            
def puzzle(D1,D2):
    D2right=[]
    n=1
    for i in D2[0]:
        if n%2==0:
            D2right.append(i)
        n=n+1    
    c=D1[0][:]
    d=D2right[:]
    d.sort()
    d.reverse()
    Ans={}
    flag=0
    flag5=0
    for i in d:
        if i<10 and (i in c):
            if i in Ans.keys():
                Ans[i].append([i])
            else:
                Ans[i]=[[i]]    
            c.remove(i)
            a=[c]
        else:    
            a=SumofTwo(c,float(i))
            if a[1]==-1:
                a=SumofThree(c,float(i))
                flag=flag+1
                if a[1]==-1:
                    a=SumofFour(c,float(i))
                    flag5=flag5+1
                    if i in Ans.keys():
                        Ans[i].append([a[1],a[2],a[3],a[4]])
                    else:   
                        Ans[i]=[[a[1],a[2],a[3],a[4]]]           
                else:
                    flag=flag+1
                    if i in Ans.keys():
                        Ans[i].append([a[1],a[2],a[3]])
                    else:   
                        Ans[i]=[[a[1],a[2],a[3]]]
            else:    
                if i in Ans.keys():
                    Ans[i].append([a[1],a[2]])
                else:   
                    Ans[i]=[[a[1],a[2]]]        
        c=a[0] 
    print Ans                    
    Ans2={}        
    e=d[:]
    e.reverse()
    c=D1[0][:]
    flag1=0
    flag2=0
    flag6=0
    if flag>=1:
        for i in e:
            a=SumofTwo(c,float(i))
            if a[1]==-1:
                a=SumofThree(c,float(i))
                flag1=flag1+1
                if a[1]==-1:
                    a=SumofFour(c,float(i))
                    flag6=flag6+1
                    if a[1]==-1:
                        flag2=flag2+1
                    if i in Ans2.keys():
                        Ans2[i].append([a[1],a[2],a[3],a[4]])
                    else:   
                        Ans2[i]=[[a[1],a[2],a[3],a[4]]]           
                else:
                    flag1=flag1+1
                    if i in Ans2.keys():
                        Ans2[i].append([a[1],a[2],a[3]])
                    else:   
                        Ans2[i]=[[a[1],a[2],a[3]]]
            else:    
                if i in Ans2.keys():
                    Ans2[i].append([a[1],a[2]])
                else:   
                    Ans2[i]=[[a[1],a[2]]]   
            c=a[0] 
    print Ans2                        
    if flag1<flag and flag2==0 and Ans2!={} and flag6<flag5:
        for i in D2right:
            Ans[i]=Ans2[i]    
    for i in D2right:
        if len(Ans[i])>=2:
            if len(Ans[i][0])==3:
                print str(i)+' = '+str(Ans[i][0][0])+'+'+str(Ans[i][0][1])+'+'+str(Ans[i][0][2])
            elif len(Ans[i][0])==4:
                print str(i)+' = '+str(Ans[i][0][0])+'+'+str(Ans[i][0][1])+'+'+str(Ans[i][0][2])+'+'+str(Ans[i][0][3])    
            elif len(Ans[i][0])==2:
                print str(i)+' = '+str(Ans[i][0][0])+'+'+str(Ans[i][0][1])
            else:
                print str(i)+' ='+str(Ans[i][0][0])    
            Ans[i].pop(0)
        else: 
            if len(Ans[i][0])==3:
                print str(i)+' = '+str(Ans[i][0][0])+'+'+str(Ans[i][0][1])+'+'+str(Ans[i][0][2])
            elif len(Ans[i][0])==4:
                print str(i)+' = '+str(Ans[i][0][0])+'+'+str(Ans[i][0][1])+'+'+str(Ans[i][0][2])+'+'+str(Ans[i][0][3])    
            elif  len(Ans[i][0])==2:
                print str(i)+' = '+str(Ans[i][0][0])+'+'+str(Ans[i][0][1])
            else:
                print str(i)+' ='+str(Ans[i][0][0])                        
    return 0

#if __name__ == "__main__":
    #code for checking output for single test input
#    fo = open("test_inputs/Test_input0.txt", "r")
#    D1=[map(int, fo.readline().split())]
 #   D2=[map(int, fo.readline().split())]
 #   puzzle (D1,D2)

    #code for checking output for all test inputs
  #  for file_number in range(0,3):
   #     file_name = "test_inputs/Test_input"+str(file_number)+".txt"
    #    fo = open(file_name, "r")
     #   D1=[map(int, fo.readline().split())]
     #   D2=[map(int, fo.readline().split())]
      #  puzzle (D1,D2)
puzzle([0,3,5,8,6,4,3,1,6,1,2,4],[7,15,12,19])      
