#!/usr/bin/env python
import re;
import string;
	
class attrdict(dict):
    #c=0
    #names=[]
    #values=[]
    #def get(id):
    #	if id<len(names):
    #		return values[id]
    def __getattr__(self, name):
	return self[name]

    def __setattr__(self, name, value):
	self[name] = value
    def add(self, name, value):
	if isinstance(name, list):
		#print len(name)
		for i in range(0,len(name)):
			n=name[i]
			if i<len(value):
				v=value[i]
			else:
				v=value[len(value)-1]
			self[n] = v
	else:
		self[name] = value
    def inkeys(self, name):
	return name in self.keys()
    def invalues(self, name):
	return name in self.values()
    def indexkeys(self,i):
	self.keys().index(i)
    def indexvalues(self,i):
	self.values().index(i)
    def countkeys(self,i):
	self.keys().count(i)
    def countvalues(self,i):
	self.values().count(i)
    def dump(self):
	k=self.keys()
	v=self.values()
	for i in range(0,len(k)):
			print k[i]+'='+v[i]

constellationnames=[
'Andromeda',
'Antlia',
'Apus',
'Aquarius',
'Aquila',
'Ara',
'Aries',
'Auriga',
'Bootes',
'Caelum',
'Camelopardus',
'Cancer',
'Canes Venatici',
'Canis Major',
'Canis Minor',
'Capricornus',
'Carina',
'Cassiopeia',
'Centaurus',
'Cepheus',
'Cetus',
'Chamaeleon',
'Circinus',
'Columba',
'Coma Berenices',
'Corona Australis',
'Corona Borealis',
'Corvus',
'Crater',
'Crux',
'Cygnus',
'Delphinus',
'Dorado',
'Draco',
'Equuleus',
'Eridanus',
'Fornax',
'Gemini',
'Grus',
'Hercules',
'Horologium',
'Hydra',
'Hydrus',
'Indus',
'Lacerta',
'Leo',
'Leo Minor',
'Lepus',
'Libra',
'Lupus',
'Lynx',
'Lyra',
'Mensa',
'Microscopium',
'Monoceros',
'Musca',
'Norma',
'Octans',
'Ophiuchus',
'Orion',
'Pavo',
'Pegasus',
'Perseus',
'Phoenix',
'Pictor',
'Pisces',
'Pisces Austrinus',
'Puppis',
'Pyxis',
'Reticulum',
'Sagitta',
'Sagittarius',
'Scorpius',
'Sculptor',
'Scutum',
'Serpens',
'Sextans',
'Taurus',
'Telescopium',
'Triangulum',
'Triangulum Australe',
'Tucana',
'Ursa Major',
'Ursa Minor',
'Vela',
'Virgo',
'Volans',
'Vulpecula']

constellationabbr=['And','Ant','Aps','Aqr','Aql','Ara','Ari','Aur','Boo','Cae','Cam','Cnc','CVn','CMa','CMi','Cap','Car','Cas','Cen','Cep','Cet',
		'Cha','Cir','Col','Com','CrA','CrB','Crv','Crt','Cru','Cyg','Del','Dor','Dra','Equ','Eri','For','Gem','Gru','Her','Hor','Hya',
		'Hyi','Ind','Lac','Leo','LMi','Lep','Lib','Lup','Lyn','Lyr','Men','Mic','Mon','Mus','Nor','Oct','Oph','Ori','Pav','Peg','Per',
		'Phe','Pic','Psc','PsA','Pup','Pyx','Ret','Sge','Sgr','Sco','Scl','Sct','Ser','Sex','Tau','Tel','Tri','TrA','Tuc','UMa','UMi',
		'Vel','Vir','Vol','Vul'
		]

#constellationabbr2=[Apu,Aqu]

gl=['alpha','beta','gamma','delta','epsilon','zeta','eta','theta','iota','kappa','lambda','mu','nu','xi','omicron','pi',
'rho','sigma','tau','upsilon','phi','chi','psi','omega','unamed','unnamed']




def readcatalog(catalog,catalog2,catalog3,catalog4):
	f=open('catalog', 'r')
	i=0
	for line in f:
		#if (i==5510):
		#	print line
		#catalogentry=attrdict()
		#catalogentry.name=line[4:13]
		XX='''
		equinox=60#75#60
		
		coords=line[equinox+0:equinox+15]
		if coords.isspace():
			continue
		
		rah=line[equinox+0:equinox+2]
		ram=line[equinox+2:equinox+4]
		ras=line[equinox+4:equinox+8]#.
		dec=line[equinox+8:equinox+11]#-+
		decm=line[equinox+11:equinox+13]
		decs=line[equinox+13:equinox+15]
		'''
		X='''
		print rah
		print ram
		print ras

		cl= "%d %d %f\n" %(int(rah),int(ram),float(ras))
		print cl

		print dec
		print decm
		print decs

		cl= "%d %d %d\n" %(int(dec),int(decm),int(decs))
		print cl
		'''
		XXX='''
		catalogentry.ra=((float(ras)/60.0+int(ram))/60.0+int(rah))*15.0
		
		if(int(dec)<0):
			catalogentry.dec=-1.0*(int(decs)/60.0+int(decm))/60.0+int(dec)
		else:
			catalogentry.dec=(int(decs)/60.0+int(decm))/60.0+int(dec)
		#print catalogentry.name
		#catalog.append(catalogentry)
		'''
		catalog.append(line[4:14])
		
		#  52- 60  A9	  ---	  VarID    Variable star identification

		catalog2.append(line[51:60])
		#print line[7:14]
		catalog3.append(line[7:14])
		#print line[7:10]+line[11:14]
		catalog4.append(line[7:10]+line[11:14])
		'''
		if (i==5510):
			print line[4:14]
		
		if (i==5510):
			print line[7:14]
		if (i==5510):
			print line[51:60]
		'''
		i=i+1
	#print i	
			
def readconstellationlines(file,catalog,catalog2,catalog3,catalog4):
	cl= ";%s" %(file)
	print cl
	f=open(file, 'r')
	started=False
	#print 1
	constellation=""
	cn=''
	cmd=0
	name=""
	for line in f:
		if line[0]=='/':
			continue
		if line.isspace():
			continue
		p = re.compile('\s*\d+\s+(\d)\s+end\s*\n')
		m = p.match( line )
		if m:
			continue

		p = re.compile('\s*;\s*(.*)\s*\n')
		m = p.match( line )
		if m:
			constellation=m.group(1);
			#cl= "\'%s\'," %(constellation)
			#print cl
			
			if(constellation=="Andromeda"):
				started=True
				#print line
				
			if(constellation=="End of file"):
				break
		else:		
			str1=''
			str2=''
			str3=''
			str4=''
			str5=''
			if started==True:			
				p = re.compile('\s*(\d+)\s+(\d)\s+draw to (.*)\n')
				m = p.match( line )
				if m:
					cn=m.group(1)					
					cmd=m.group(2)
					name=m.group(3)
					#cl= "%d %s\n" %(int(cmd),name)
					#print cl
				p = re.compile('\s*(\d+)\s+(\d)\s+move to (.*)\n')
				m = p.match( line )
				if m:
					cn=m.group(1)					
					cmd=m.group(2)
					name=m.group(3)
					#cl= "%d %s\n" %(int(cmd),name)
					#print cl
				p = re.compile('\s*(\d+)\s+(\d)\s+draw dotted to (.*)\n')
				m = p.match( line )
				if m:
					cn=m.group(1)					
					cmd=m.group(2)
					name=m.group(3)
					#cl= "%d %s\n" %(int(cmd),name)
					#print cl
				p = re.compile('\s*(\d+)\s+(\d)\s+draw dashed to (.*)\n')
				m = p.match( line )
				if m:
					cn=m.group(1)					
					cmd=m.group(2)
					name=m.group(3)
					#cl= "%d %s\n" %(int(cmd),name)
					#print cl
				p = re.compile('\s*(\d+)\s+(\d)\s+dotted to (.*)\n')
				m = p.match( line )
				if m:
					cn=m.group(1)					
					cmd=m.group(2)
					name=m.group(3)
					#cl= "%d %s\n" %(int(cmd),name)
					#print cl
				p = re.compile('\s*(\d+)\s+(\d)\s+draw to unnamed (.*)\n')#???unnamed???
				m = p.match( line )
				if m:
					cn=m.group(1)					
					cmd=m.group(2)
					name=m.group(3)
					#cl= "%d %s\n" %(int(cmd),name)
					#print cl


				if name in gl:
					str1=''
					if(len(name)==2):
						str2="%s "%(name[0:2].capitalize())
					else:
						str2=name[0:3].capitalize()
					str3=' '
					#str4=constellation[0:3].capitalize()#???
					#print constellation
					str4=constellationabbr[constellationnames.index(constellation)] 				
					str5= "%3s %3s	" %(str2,str4)
				else:	
					p = re.compile('(\w+)\s+(\w+)')
					m = p.match( name )
					if m:
						str1='	 '						

						if(len(name)==2):
							str2="%s "%(name[0:2].capitalize())
						else:
							str2=name[0:3].capitalize()
						
						str=m.group(2)
						if str in constellationabbr:
							str3=' '
							str4=str#str[0:3].capitalize()
						else:	
							str3=str[0:1]
							#str4=constellation[0:3].capitalize()#???
							str4=constellationabbr[constellationnames.index(constellation)] 				
					
						str5= "%3s %3s	" %(str2,str4)	
					else:		
						p = re.compile('\w+')
						m = p.match( name )
						if m:
							str1=name[0:3]#leading spaces???
							str2='	 '
							str3=' '
							#str4=constellation[0:3].capitalize()#???
							str4=constellationabbr[constellationnames.index(constellation)] 				
					
							str5= "%3s %3s	" %(str1,str4)


# 25Psi1Ori
			
			
			#cl= "%d %3s%3s%1s%3s" %(int(cmd),str1,str2,str3,str4)
			cl= "%3s%3s%1s%3s" %(str1,str2,str3,str4)
			
			#print cl
			#print str5
			str6=str5[0:7]
			str7=str2+str4
			#print str6
			
			
			k=-1
			try:
				k = catalog.index(cl)
			except ValueError:
				k = -1 # no match
			if(k==-1):
				try:
					k = catalog2.index(str5)
				except ValueError:
					k = -1 # no match
			if(k==-1):
				try:
					k = catalog3.index(str6)
				except ValueError:
					k = -1 # no match
			if(k==-1):
				try:
					k = catalog4.index(str7)
				except ValueError:
					k = -1 # no match



			line2=''
			p = re.compile('(.*)\n')
			m = p.match( line )
			if m:
			   line2=m.group(1)

			#if ((cl in catalog) or (str5 in catalog2) or (str6 in catalog3) or (str7 in catalog4)):
			if(k!=-1):



				#cl2="%s found" %(cl)
				cl2="%s %d\t%s" %(cmd,int(k),line2)#idx???
				print cl2

			else:
				#cl2="%s not found" %(cl)
				cl2="//%s %s\t%s" %(cmd,cn,line2)#cn???
				print cl2
			


'''
def testcatalog():			
	catalog=[]
	catalog2=[]
	readcatalog(catalog)
	for catalogentry in catalog:
		cl= "%s\n" %(catalogentry)
		print cl
'''
#testcatalog()
X='''
readconstellationlines("Constellations/ConstellationFigures/dhmenz11/DHMENZEL.TXT");
readconstellationlines("Constellations/ConstellationFigures/Karkoschka_cf/Karkoschka.txt");
readconstellationlines("Constellations/ConstellationFigures/rukl_cf/rukl.txt");
readconstellationlines("Constellations/ConstellationFigures/Skywatch8/Skywatch.txt");
readconstellationlines("Constellations/ConstellationFigures/hareyv8/harconst.txt");
readconstellationlines("Constellations/ConstellationFigures/dhmenzv8/DHMENZEL.TXT");
readconstellationlines("Constellations/ConstellationFigures/hareyv5/harconst.txt");
readconstellationlines("Constellations/ConstellationFigures/dhmenzv5/DHMENZEL.TXT");
readconstellationlines("Constellations/ConstellationFigures/Skywatch/Skywatch.txt");
readconstellationlines("Constellations/ConstellationFigures/harey/harconst.txt");
readconstellationlines("Constellations/ConstellationFigures/dhmenzel/DHMENZEL.TXT");
'''
catalog=[]
catalog2=[]
catalog3=[]
catalog4=[]
readcatalog(catalog,catalog2,catalog3,catalog4)
readconstellationlines("Constellations/ConstellationFigures/dhmenz11/DHMENZEL.TXT",catalog,catalog2,catalog3,catalog4)

readconstellationlines("Constellations/ConstellationFigures/Karkoschka_cf/Karkoschka.txt",catalog,catalog2,catalog3,catalog4);
readconstellationlines("Constellations/ConstellationFigures/rukl_cf/rukl.txt",catalog,catalog2,catalog3,catalog4);
readconstellationlines("Constellations/ConstellationFigures/Skywatch8/Skywatch.txt",catalog,catalog2,catalog3,catalog4);
readconstellationlines("Constellations/ConstellationFigures/hareyv8/harconst.txt",catalog,catalog2,catalog3,catalog4);
readconstellationlines("Constellations/ConstellationFigures/dhmenzv8/DHMENZEL.TXT",catalog,catalog2,catalog3,catalog4);
readconstellationlines("Constellations/ConstellationFigures/hareyv5/harconst.txt",catalog,catalog2,catalog3,catalog4);
readconstellationlines("Constellations/ConstellationFigures/dhmenzv5/DHMENZEL.TXT",catalog,catalog2,catalog3,catalog4);
readconstellationlines("Constellations/ConstellationFigures/Skywatch/Skywatch.txt",catalog,catalog2,catalog3,catalog4);
readconstellationlines("Constellations/ConstellationFigures/harey/harconst.txt",catalog,catalog2,catalog3,catalog4);
readconstellationlines("Constellations/ConstellationFigures/dhmenzel/DHMENZEL.TXT",catalog,catalog2,catalog3,catalog4);
print ';'


