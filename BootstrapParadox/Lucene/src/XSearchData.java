import org.apache.lucene.analysis.*;
import org.apache.lucene.analysis.standard.*;
import org.apache.lucene.util.Version;
import java.nio.file.Files;
import java.nio.file.Paths;
import org.apache.lucene.store.*;
import org.apache.lucene.index.*;
import org.apache.lucene.search.*;
import org.apache.lucene.util.*;
import org.apache.lucene.document.*;
import java.io.*;
import java.util.*;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;


class XSearchData implements Runnable {
    
    private Document doc;
    
     private static IndexWriter writer;
    
    public XSearchData(Document d)
    {
        this.doc = d;
    }
    
     @Override
    public void run() {
         try {
      //     System.out.println(this.doc.toString());
        writer.addDocument(this.doc);
        
    } catch (Exception e) {
        System.out.println(e.toString());
    }
    }
    
    public static void main(String[] args) throws Exception {
        Analyzer analyzer;
       // RAMDirectory directory;
       String indexDir = "/home/cc/xsearch/Lucene/idx";
       Directory directory = 
         FSDirectory.open(Paths.get(indexDir)); 
        IndexWriterConfig config;
        int cores = Runtime.getRuntime().availableProcessors();
        System.out.println("cores::"+48);
        ExecutorService executor = Executors.newFixedThreadPool(48);

        DirectoryReader ireader;
        IndexSearcher isearcher;
        QueryBuilder builder;
        RandomAccessFile in;
        ArrayList<String> inputFiles;
        String termsFile;
        ArrayList<Document> documents;
        String line;
        long start, end;
        long indexTime, indexSize, searchTime;

        indexTime = 0;
        indexSize = 0;
        searchTime = 0;
        try {
            // read the file paths from the input file
            inputFiles = new ArrayList<String>();
            in = new RandomAccessFile(args[0], "r");
            while ((line = in.readLine()) != null) {
                inputFiles.add(line);
            }
            in.close();
            
            // create a list of document that are going to be indexed
            documents = new ArrayList<Document>();
            for (String inputFile : inputFiles) {
                File file = new File(inputFile);
                Document document = new Document();

                Field contentField = new Field("content", new InputStreamReader(new FileInputStream(file)),
                        TextField.TYPE_NOT_STORED);
                Field filenameField = new Field("filename", file.getName(), StoredField.TYPE);
                Field filepathField = new Field("filepath", file.getCanonicalPath(), StoredField.TYPE);

                document.add(contentField);
                document.add(filenameField);
                document.add(filepathField);

                documents.add(document);
            }

            // use the standard text analyzer
            analyzer = new StandardAnalyzer();

            // store the index in main memory (RAM)
           // directory = new RAMDirectory();

            // create and index writer
            config = new IndexWriterConfig(analyzer);
            writer = new IndexWriter(directory, config);
           config.setRAMBufferSizeMB(1024.0);
           System.out.println("max buffered docs::"+ config.getMaxBufferedDocs());
           System.out.println("max buffer size::"+config.getRAMBufferSizeMB()) ; 
          //  config.setMaxBufferedDocs(config.getMaxBufferedDocs()/2);
            // index each file one by one and measure the time taken
            start = System.currentTimeMillis();
            for (Document document : documents) {
                //iwriter.addDocument(document);
                Runnable worker = new XSearchData(document);
                executor.execute(worker);
                
            }
            
            executor.shutdown();
            while (!executor.awaitTermination(24L, TimeUnit.HOURS)) {
            //System.out.println("Not yet. Still waiting for termination");
            }
            
             writer.commit();
            writer.close();
           
            end = System.currentTimeMillis();
            
            // calculate the time taken to index the files
            indexTime = (end - start);

            // get the total size of the index
         //   indexSize = directory.ramBytesUsed() / 1000;

            // create an index reader
            ireader = DirectoryReader.open(directory);
            isearcher = new IndexSearcher(ireader);
            builder = new QueryBuilder(analyzer);

            // read the terms from the second input file and search the index
            in = new RandomAccessFile(args[1], "r");
            start = System.currentTimeMillis();
            while ((line = in.readLine()) != null) {
                Query query = builder.createBooleanQuery("content", line);
                //System.out.println(query.toString());
                ScoreDoc[] hits = isearcher.search(query, 1000).scoreDocs;
                // uncomment these lines to check the correctness of the search results
                //if (hits.length < 1) {
                //    System.out.println("Incorrect search result!");
                //}
            }
            end = System.currentTimeMillis();
            in.close();

            ireader.close();
            directory.close();

            // calculate the time it took to search all the terms
            searchTime = (end - start);
        } catch (IOException e) {
            e.printStackTrace();
        }

        System.out.println("IndexTime: " + indexTime + " ms");
        System.out.println("IndexSize: " + indexSize + " kB");
        System.out.println("SearchTime: " + searchTime + " ms");

        System.exit(0);  
    }
}
