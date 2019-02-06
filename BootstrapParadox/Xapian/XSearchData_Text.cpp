

#define NUM_THREADS 4

void parallel_index(int tid, int num_threads,vector<string> documents,Xapian::WritableDatabase *db )
{
    int i =0;
     char line[2048];
     string line_string;
     Xapian::TermGenerator indexer;
	Xapian::Stem stemmer("english");
	
    indexer.set_stemmer(stemmer);
  //  doc.set_data("content");
    indexer.set_stemming_strategy(Xapian::TermGenerator::STEM_SOME);
   Xapian::WritableDatabase database = *db;
   try{
   for(i=0;i<documents.size();++i)
   {    
      
       ifstream file;
       file.open(documents[i]);
       line_string = "";
       while(file.good())
       {
            
                       file.getline(line, sizeof(line));
                       string str(line);
                       if(!str.empty())
                       {
                        line_string += string(line);
                       }
       }
       
       Xapian::Document doc;
		doc.set_data(line_string);

		indexer.set_document(doc);
		indexer.index_text(line_string);
       database.add_document(doc); 

       file.close();
   }
   database.commit();
   cout <<"count::" <<database.get_doccount() <<endl;
   }
   catch (const Xapian::Error &e) {
    cout << e.get_description() << endl;
    exit(1);
}
   
   
}


int main(int argc, char **argv)
{
    vector<thread> threads;
    int doc_ct = 0;
  // char queries[][20] = {"Probation","Volleyball"};
   int th = 0;
   vector<string> documents[NUM_THREADS];
    Xapian::WritableDatabase dbs[NUM_THREADS];
   for(th=0;th<NUM_THREADS;++th)
   {
       //string s = argv[1] + to_string("\\")+to_string(th);
       char s[30] ;
       strcpy(s,argv[2]);
       strcat(s,"/DB");
       string idx = to_string(th);
       strcat(s,idx.c_str());
       
       cout << s << endl;
        dbs[th] =  Xapian::WritableDatabase(s,Xapian::DB_CREATE_OR_OVERWRITE, 512);
   }
   

    Xapian::TermGenerator indexer;
	Xapian::Stem stemmer("english");

	
   
    string line_string;
    char filename[100];
     ifstream infs;
    char *tok, filepath[1536], content[2048], line[2048];
    int i;
	high_resolution_clock::time_point start, end;
	infs.open(argv[1], ifstream::in);
	indexer.set_stemmer(stemmer);
    
    start = high_resolution_clock::now();  

    
    while(infs.good())
    {
        doc_ct = doc_ct + 1;
        infs.getline(filename, sizeof(filename));
        documents[doc_ct%NUM_THREADS].push_back(filename);
        
    }
    
      for (int tid = 0; tid < NUM_THREADS; tid++) {
            threads.push_back(thread(parallel_index, tid, NUM_THREADS, 
                    documents[tid],&dbs[tid]));
        }
        
        for (auto& th : threads) {
            th.join();
        }

   
    infs.close();
    Xapian::Database final_db;
     for(const Xapian::WritableDatabase &d : dbs)
    {
        final_db.add_database(d);
    } 
    Xapian::Enquire enquire(final_db);
	Xapian::QueryParser parser;
    end = high_resolution_clock::now();  
    auto diff1 = duration_cast<milliseconds>(end - start).count();
	cout << "Indexing time: " << diff1 << "ms" << endl;
    // search
    parser.set_stemmer(stemmer);
		parser.set_database(final_db);
		parser.set_stemming_strategy(Xapian::QueryParser::STEM_SOME);

		start = high_resolution_clock::now();
        ifstream searchFile;
        searchFile.open(argv[3]);
        char word[40];
		while(searchFile.good()) {
            searchFile.getline(word, sizeof(word));
			cout << "Searching : " <<  word<< endl ;
			Xapian::Query query = parser.parse_query(word);
			enquire.set_query(query);
			
			
			Xapian::MSet hits = enquire.get_mset(0, 10);
			
	//cout << "Parsed query is: " << query.get_description() << endl;

  
    
    // Display the results.
    cout << hits.get_matches_estimated() << " results found.\n";
    cout << "Matches 1-" << hits.size() << ":\n" << endl;
			
		}
		
		end = high_resolution_clock::now();
		
		auto diff = duration_cast<milliseconds>(end - start).count();
		cout << "Average search time: " << diff*1000 << "ns" << endl;
	
    } 
