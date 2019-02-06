#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <boost/tokenizer.hpp>
#include <bits/stdc++.h> 
#include <xapian.h>

using namespace std;
using namespace std::chrono;

extern "C" {
        #include <string.h>
}

#define NUM_THREADS 4

void parallel_index(int tid, int num_threads,vector<string> documents,Xapian::WritableDatabase *db )
{
    cout<<documents.size()<<endl;
    int i =0;
     char line[2048];
     string line_string;
     Xapian::TermGenerator indexer;
	Xapian::Stem stemmer("english");
	
    indexer.set_stemmer(stemmer);
  //  doc.set_data("content");
    
   Xapian::WritableDatabase database = *db;
   indexer.set_database(database);
   
   for(i=0;i<documents.size();++i)
   {    
       
       ifstream infs;
       infs.open(documents[i]);
       line_string = "";
        while(infs.good()) {
            try{
        vector <string> tokens; 
                       
                        
                       // char idterm[20];
                        string idterm;
                        Xapian::Document doc;
                        indexer.set_document(doc);
                        infs.getline(line, sizeof(line));
                        string ln_str(line);
                        string intermediate; 
                        
                         stringstream check1(ln_str);
                        //indexer.set_document(doc);
                        
                        while(getline(check1, intermediate, ' ')) 
                        { 
                            tokens.push_back(intermediate); 
                        }
                        if(tokens.size() >=16)
                        {
                            
                            idterm = "Q" + std::string(tokens[0]);
                            doc.add_boolean_term(idterm);
                            indexer.index_text(tokens[15], 1, "P");
                             indexer.increase_termpos();
                        }
                         //   indexer.index_text(ln_str);
                           // indexer.increase_termpos();
                            database.replace_document(idterm, doc);
                       // cout << "count::"<< c << endl;
        }           
        catch (const Xapian::Error &e) {
		//cout << e.get_description() << endl;
	}
    }
    infs.close();
       }
       
       
        
       
        database.commit();
   }
   
   
   
  
 

   
   


int main(int argc, char **argv)
{
    vector<thread> threads;
    high_resolution_clock::time_point start, end;
    int doc_ct = 0;
    
    vector<string> documents[NUM_THREADS];
    Xapian::WritableDatabase dbs[NUM_THREADS];
    Xapian::Stem stemmer("english");
	//Xapian::Enquire enquire(db);
     char filename[100];
     
	Xapian::QueryParser parser;
	ifstream infs;
    char *tok, filepath[1536], content[2048], line[2048];
    infs.open(argv[1], ifstream::in);
    int c = 0;
    
     start = high_resolution_clock::now(); 
   for(int th=0;th<NUM_THREADS;++th)
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
   
   while(infs.good())
    {
        char path[100] =  argv[3];
        doc_ct = doc_ct + 1;
        infs.getline(filename, sizeof(filename));
        strcat(path,filename);
        documents[doc_ct%NUM_THREADS].push_back(path);
        
    }
    
    for (int tid = 0; tid < NUM_THREADS; tid++) {
            threads.push_back(thread(parallel_index, tid, NUM_THREADS, 
                    documents[tid],&dbs[tid]));
        }
        
        for (auto& th : threads) {
            th.join();
        }
        
   
  infs.close();
    
                cout << "done index"<< endl;
                
     Xapian::Database final_db;
     for(const Xapian::WritableDatabase &d : dbs)
    {
        final_db.add_database(d);
    } 
    end = high_resolution_clock::now();  
    auto diff1 = duration_cast<milliseconds>(end - start).count();
	cout << "Indexing time: " << diff1 << "ms" << endl;
    
    
    start = high_resolution_clock::now();
    
    Xapian::Enquire enquire(final_db);
     //search
     
     parser.set_stemmer(stemmer);
		parser.set_database(final_db);
		parser.set_stemming_strategy(Xapian::QueryParser::STEM_SOME);
         ifstream searchFile;
        searchFile.open(argv[3]);
        char word[40];
		while(searchFile.good()) {
            searchFile.getline(word, sizeof(word));
        parser.add_prefix("Pathname","P");
       Xapian::Query query = parser.parse_query(word);
        enquire.set_query(query);
        Xapian::MSet hits = enquire.get_mset(0, 1000);
            cout << hits.get_matches_estimated() << " results found.\n";
    cout << "Matches 1-" << hits.size() << ":\n" << endl;
        }
    end = high_resolution_clock::now();
    diff1 = duration_cast<milliseconds>(end - start).count();
	cout << "Indexing time: " << diff1 << "ms" << endl;
    
    return 0;
}

