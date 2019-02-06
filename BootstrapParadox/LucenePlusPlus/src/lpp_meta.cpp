#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <exception>
#include <LuceneHeaders.h>
#include <ConcurrentMergeScheduler.h>
#include <FileUtils.h>
#include <MiscUtils.h>

extern "C" 
{
    #include <string.h>
    #include <sys/time.h>
}

#define NUM_THREADS 1


using namespace std;
using namespace Lucene;

void parallel_index(int tid, int num_threads,vector<string> documents)
{
    //code for indexing
        string idxdir = "/home/cc/xsearch/LucenePlusPlus/idx/id"+to_string(tid);
        
      DirectoryPtr directory = FSDirectory::open(StringUtils::toUnicode(idxdir));
      AnalyzerPtr  analyzer = newLucene<StandardAnalyzer>(LuceneVersion::LUCENE_CURRENT);
      IndexWriterPtr iwriter = newLucene<IndexWriter>(directory, analyzer, true,
                IndexWriter::MaxFieldLengthUNLIMITED);
          //      cout<<"hello::"<<tid<<endl;
          cout<<"docs::"<<documents.size()<<endl;
      for(int i=0;i<documents.size();++i)
      {
          //cout<<"hello::"<<tid<<endl;
        ifstream file;
       // cout << documents[i]<<endl;
        file.open(documents[i]);
        while(file.good())
        {
            DocumentPtr document = newLucene<Document>();
            vector <string> tokens; 
            char line[2048];
            file.getline(line,sizeof(line));
            string ln_str(line);
            string intermediate; 
                         stringstream check1(ln_str);
                        //indexer.set_document(doc);
            while(getline(check1, intermediate, ' ')) 
            { 
                tokens.push_back(intermediate); 
            }
            if(tokens.size()>=16)
            {
                char in[100];
                char filename[1024];
                
                strcpy(in, tokens[0].c_str());
                strcpy(filename, tokens[15].c_str());
                
                String inode(StringUtils::toUnicode(in));
                String file(StringUtils::toUnicode(filename));
                
            
               document->add(newLucene<Field>(L"inode", inode,
                    Field::STORE_YES, Field::INDEX_NOT_ANALYZED));
                    
                    document->add(newLucene<Field>(L"filename", file,
                    Field::STORE_YES, Field::INDEX_NOT_ANALYZED));
            /* for(int i = 0; i < tokens.size(); i++) 
            {
                if(i == 0)
                {
                    document->add(newLucene<Field>(L"inode", StringUtils::toUnicode(tokens[i])));
                    
                }
                if(i == 15)
                {
                    //pathname
                    document->add(newLucene<Field>(L"filename", StringUtils::toUnicode(tokens[i])));
                }
            } */
            iwriter->addDocument(document);
            }
        }
      }
      iwriter->commit();
        directory->close();
      
}

int main(int argc, char **argv)
{
    vector<string> documents[NUM_THREADS];
    AnalyzerPtr analyzer = newLucene<StandardAnalyzer>(LuceneVersion::LUCENE_CURRENT);
    IndexReaderPtr ireader;
    IndexSearcherPtr isearcher;
    QueryParserPtr parser;
    ifstream in;
    vector<string> inputFiles;
   // vector<DocumentPtr> documents;
    struct timeval start, end;
    long indexTime, indexSize, searchTime;
    ConcurrentMergeSchedulerPtr scheduler;
    vector<thread> threads;
    ifstream infs;
    infs.open(argv[1], ifstream::in);
    int doc_ct=0;
    char filename[100];
    gettimeofday(&start, NULL);
    while(infs.good())
    {
        doc_ct = doc_ct + 1;
        infs.getline(filename, sizeof(filename));
        char filepath[1024] = argv[2];
        strcat(filepath,filename);
      //  cout<<filepath<<endl;
        documents[doc_ct%NUM_THREADS].push_back(filepath);
        
    }
    
    indexTime = 0;
    indexSize = 0;
    searchTime = 0;
    
        for (int tid = 0; tid < NUM_THREADS; tid++) {
            threads.push_back(thread(parallel_index, tid, NUM_THREADS,documents[tid]));
        }
        for (auto& th : threads) {
            th.join();
        }
       
        gettimeofday(&end, NULL);
        cout << "index completed"<<endl;
     indexTime += (((long) end.tv_sec - (long) start.tv_sec) 
                * 1000000 + (end.tv_usec - start.tv_usec)) / 1000;
                cout <<"index time::"<<indexTime<<endl;
gettimeofday(&start, NULL);
            ifstream searchFile;
             searchFile.open(argv[3]);
            
        char word[40];
		while(searchFile.good()) {
            
            searchFile.getline(word, sizeof(word));
         //     cout<<"word::"<<word<<endl;
            string qry(word);
            cout<<qry<<endl;
            for(int i=0;i<NUM_THREADS;++i)
            {
                 string idxdir = "/home/cc/xsearch/LucenePlusPlus/idx/id"+std::to_string(i);
                 
                DirectoryPtr dir = FSDirectory::open(StringUtils::toUnicode(idxdir));
                 ireader = IndexReader::open(dir);
        isearcher = newLucene<IndexSearcher>(ireader);
        parser = newLucene<QueryParser>(LuceneVersion::LUCENE_CURRENT,
                L"inode", analyzer);
                 String term(StringUtils::toUnicode(qry));
            QueryPtr query = parser->parse(term);
            
            Collection<ScoreDocPtr> hits = isearcher->search(
                    query, 1000)->scoreDocs;
           
            }
        }   
        
        gettimeofday(&end, NULL);
 
        isearcher->close();
        
        
        // calculate the time it took to search all the terms
        searchTime += (((long) end.tv_sec - (long) start.tv_sec) 
                * 1000000 + (end.tv_usec - start.tv_usec)) / 1000;
    
    
in.close();
    cout << "IndexTime: " << indexTime << " ms" << endl;
    cout << "IndexSize: " << indexSize << " kB" << endl;
    cout << "SearchTime: " << searchTime << " ms" << endl;

    return 0;
}
