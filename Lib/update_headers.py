import os,sys

dr = {	" EXPORT ":" ",
		"PRIVATE_INTERNAL":"private",
		"PROTECTED_INTERNAL":"protected",
	}

s = ""
path = os.path.dirname(os.path.dirname(sys.argv[0]))
result = sys.argv[1]
if os.path.exists(result):
	try:
		os.unlink(result)
	except Exception as e:
		print("[ERROR] - Unable to delete: "+result)
		print(e)
		raise SystemExit	

try:
	add = True
	for line in open(os.path.join(path,"GAppFramework","GApp.h"),"rt"):
		if "//[REMOVE-IN-DEVELOP-LIB:START]" in line:
			add = False
			continue
		if "//[REMOVE-IN-DEVELOP-LIB:STOP]" in line:
			add = True
			continue
		line = line.replace("CONSOLE_COLOR(12);printf(\"[LOG-ERROR] \");","CONSOLE_COLOR(12);printf(\"[GAC-ERROR] \");")
		line = line.replace("CONSOLE_COLOR(7);printf(\"[LOG-INFO ] \");","CONSOLE_COLOR(7);printf(\"[GAC-INFO ] \");")
			
		if (add):
			for key in dr:
				line = line.replace(key,dr[key])
			if line.lstrip().startswith("#"):
				line = line.lstrip()
			s += line
	print("[CREATE] - "+result)
	open(result,"wt").write(s)
	print("[ALL-OK] - GApp header file created succesifully !")
	
except Exception as e:
	print("[ERROR] Exception creating GApp.h file !")
	print(e)
	raise(SystemExit)
