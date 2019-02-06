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

#define NUM_THREADS 4

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
      for(int i=0;i<documents.size();++i)
      {
        DocumentPtr document = newLucene<Document>();
        String sourceFile(StringUtils::toUnicode(documents[i]));
        std::ifstream ifs(documents[i]);
        std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
        String cont(StringUtils::toUnicode(content));
        //wcout<<cont<<endl;
        document->add(newLucene<Field>(L"content",cont,Field::STORE_YES, Field::INDEX_NOT_ANALYZED ));        
        document->add(newLucene<Field>(L"filepath", sourceFile,
                    Field::STORE_YES, Field::INDEX_NOT_ANALYZED));
        iwriter->addDocument(document);
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
    while(infs.good())
    {
        doc_ct = doc_ct + 1;
        infs.getline(filename, sizeof(filename));
       
      //  cout<<filepath<<endl;
        documents[doc_ct%NUM_THREADS].push_back(filename);
        
    }
    
    indexTime = 0;
    indexSize = 0;
    searchTime = 0;
    
    gettimeofday(&start, NULL);
        for (int tid = 0; tid < NUM_THREADS; tid++) {
            threads.push_back(thread(parallel_index, tid, NUM_THREADS,documents[tid]));
        }
        for (auto& th : threads) {
            th.join();
        }
       
        gettimeofday(&end, NULL);
        indexTime += (((long) end.tv_sec - (long) start.tv_sec) 
                * 1000000 + (end.tv_usec - start.tv_usec)) / 1000;
        cout << "index completed"<<endl;
        string fidxdir = "/home/cc/xsearch/LucenePlusPlus/idx/finalidx";
        DirectoryPtr fdirectory = FSDirectory::open(StringUtils::toUnicode(fidxdir));
        IndexWriterPtr fiwriter = newLucene<IndexWriter>(fdirectory, analyzer, true,
                IndexWriter::MaxFieldLengthUNLIMITED);
                Collection<IndexReaderPtr> readers;
              //  DirectoryPtr dir;
              
              //search
			  gettimeofday(&start, NULL);
			  ifstream searchFile;
        searchFile.open(argv[2]);
        char word[40];
		while(searchFile.good()) {
			searchFile.getline(word, sizeof(word));
			string termstring(word);
            for(int i=0;i<NUM_THREADS;++i)
            {
				
                 string idxdir = "/home/cc/xsearch/LucenePlusPlus/idx/id"+std::to_string(i);
                 cout << idxdir << endl;
                DirectoryPtr dir = FSDirectory::open(StringUtils::toUnicode(idxdir));
                 ireader = IndexReader::open(dir);
        isearcher = newLucene<IndexSearcher>(ireader);
        parser = newLucene<QueryParser>(LuceneVersion::LUCENE_CURRENT,
                L"content", analyzer);
                 String term(StringUtils::toUnicode(termstring));
            QueryPtr query = parser->parse(term);
            
            Collection<ScoreDocPtr> hits = isearcher->search(
                    query, 1000)->scoreDocs;
                    
            for(int i=0;i<hits.size();++i)
            {
                wcout << hits[i]->doc<<endl; 
                DocumentPtr doc = isearcher->doc(hits[i]->doc);
                wcout << doc->get(L"filepath");
            }
}
		}
       
           
        
        in.close();
        gettimeofday(&end, NULL);
        
        isearcher->close();
        fdirectory->close();
        
        // calculate the time it took to search all the terms
        searchTime += (((long) end.tv_sec - (long) start.tv_sec) 
                * 1000000 + (end.tv_usec - start.tv_usec)) / 1000;
    
    

    cout << "IndexTime: " << indexTime << " ms" << endl;
    cout << "IndexSize: " << indexSize << " kB" << endl;
    cout << "SearchTime: " << searchTime << " ms" << endl;

    return 0;
}
