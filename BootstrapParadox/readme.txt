=============================================

XSEARCH : BOOTSTRAP PARADOX

=============================================

The following are the steps that need to be executed in order to run the Project:
(Note : The experiments were run on chameleon baremetal instance so the paths set accordingly in the code)

Setup Lucene and LucenePlusPlus as per the document provided by Alex on https://gitlab.com/cs554-2018/xsearch and setup 
Xapian with the help of steps given on https://xapian.org/docs/install.html

1. Lucene
	a. Go to Lucene directory
	b. Run the make command
	c. Text Search :  java XSearchData  <path to input files> <file path to search the terms>
	d. Metadata Search: create the following paths - 
		1. /home/cc/xsearch/Lucene/idx
		2. /home/cc/xsearch/Lucene/idx/finalIdx
	e. Run java testMultiMeta <path to input files> <file path to search terms>


2. LucenePlusPlus
	a. Go to LucenePlusPlus directory
	b. Run the make command
	c. Text Search: create the following paths - 
		1. /home/cc/xsearch/LucenePlusPlus/idx
		2. /home/cc/xsearch/LucenePlusPlus/idx/finalidx
	d. ./bin/lpp_search.exe <path to input files> <file path to search the terms>
	e. Metadata Search: ./bin/lpp_meta.exe <path to file containing filenames> <path to directory containing input files> <file path to search the terms>
	


3. Xapian
	a. Go to Xapian directory
	b. Run the make command
	c.  create an empty folder for databases
	d. Text Search:  XSearchData_Text.exe <path to input files> <path to database> <file path to search the terms>
	e. Metadata Search: XSearchData_Meta.exe <path to input files> <path to database> <<path to directory containing input files> <file path to search the terms>
		