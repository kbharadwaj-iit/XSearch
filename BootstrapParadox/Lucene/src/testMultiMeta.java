import java.io.File;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.io.FileFilter;
import org.apache.lucene.search.Query;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import java.io.FileReader;
import java.io.IOException;
import org.apache.lucene.queryparser.classic.ParseException;
import java.io.BufferedReader;
import org.apache.lucene.document.StringField;
import java.util.concurrent.TimeUnit;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.store.Directory;
import org.apache.lucene.store.FSDirectory;
import org.apache.lucene.util.Version;
import java.nio.file.Path;
import org.apache.lucene.analysis.*;
import org.apache.lucene.analysis.standard.*;
import org.apache.lucene.store.*;
import org.apache.lucene.index.*;
import org.apache.lucene.search.*;
import org.apache.lucene.util.*;
import org.apache.lucene.document.*;
import java.io.*;
import java.util.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class testMultiMeta implements Runnable   {
	
	private ArrayList<String> documents; 
    private int docIndex;
  //  private static IndexWriter[] writer = new IndexWriter[48];
    private static int c=0;
    private static int threads =48;
	RandomAccessFile in;
     public static final String dataDir = "/home/cc/Data/meta_40/cc";
       public static final   String indexDir = "/home/cc/xsearch/Lucene/idx";
    public testMultiMeta(ArrayList<String> d,int i)
    {
        this.documents = d;
        this.docIndex = i;
    }
    
     @Override
    public void run()
    {
        try{
        ++c;
         Analyzer analyzer  = new StandardAnalyzer();
        IndexWriterConfig config = new IndexWriterConfig(analyzer);
        Directory indexDirectory = 
         FSDirectory.open(Paths.get(indexDir+"/id"+this.docIndex));
         int size = this.documents.size();
         IndexWriter writer = new IndexWriter(indexDirectory,config);
         for(int i=(this.docIndex*size/threads); i< (this.docIndex+1)*size/threads;++i)
         {
             String file = this.documents.get(i);
             BufferedReader reader;
           reader=new BufferedReader(new FileReader(file));
               String line = reader.readLine();
            while(line  != null)
            {
             Document document = new Document();
             String[] str = line.split(" ");
             if(str.length >=16)
             {
                 String inode = str[0];	  
    	  //System.out.println("inode "+inode);
    	  String user_id = str[8];
    	  //System.out.println("user "+user_id);
    	  String fileset_name = str[4];
    	  //System.out.println("fileset " +fileset_name);
    	  String filename = str[15];
    	//  System.out.println("name "+filename);
    	  line = reader.readLine();
    	  document.add(new StringField("inode", inode,Field.Store.YES));
    	  document.add(new StringField("user_id",user_id,Field.Store.YES));
    	  document.add(new StringField("fileset_name",fileset_name,Field.Store.YES));
    	  document.add(new StringField("filename",filename,Field.Store.YES));
    	  writer.addDocument(document);
             }
            }
         }
        writer.commit();
        writer.close();
        indexDirectory.close();
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
    

   public static void main(String[] args) throws ParseException{ 
       try{
          
              long start, end;
        long indexTime, indexSize, searchTime;

        indexTime = 0;
        indexSize = 0;
        searchTime = 0;
		  //threads = Runtime.getRuntime().availableProcessors();
           ExecutorService executor = Executors.newFixedThreadPool(threads);
          ArrayList<String> docs = new ArrayList<>();
          
          String line;
          RandomAccessFile in =new RandomAccessFile(args[0], "r") ;
          int cnt = 0;
          while ((line = in.readLine()) != null) {
                docs.add(dataDir+"/"+line);
            }
            start = System.currentTimeMillis();
            for(int i=0;i<threads;++i)
            {
                 Runnable worker = new testMultiMeta(docs,i);
                executor.execute(worker);
            }
        
        
     

            
            executor.shutdown();
            while (!executor.awaitTermination(24L, TimeUnit.HOURS)) {
            //System.out.println("Not yet. Still waiting for termination");
            }
            Directory indexDirectory = 
         FSDirectory.open(Paths.get("/home/cc/xsearch/Lucene/idx/finalIdx"));
        
            IndexWriterConfig config = new IndexWriterConfig(new StandardAnalyzer());
            IndexWriter writer = new IndexWriter(indexDirectory,config);
            for(int i=0;i<threads;++i)
            {
                 Directory dir = 
                FSDirectory.open(Paths.get("/home/cc/xsearch/Lucene/idx/id"+i));
               writer.addIndexes(dir);
                
            }
            end = System.currentTimeMillis();
              indexTime = (end - start);
              System.out.println("Index time::"+indexTime);
            
   writer.commit();
            writer.close();
            
            //search
            IndexSearcher isearcher;
        QueryBuilder builder;
             DirectoryReader ireader = DirectoryReader.open(indexDirectory);
            isearcher = new IndexSearcher(ireader);
            builder = new QueryBuilder(new StandardAnalyzer());
            in = new RandomAccessFile(args[1], "r");
			start = System.currentTimeMillis();
			while ((line = in.readLine()) != null) 
			{
            Query query = builder.createBooleanQuery("inode", line);
            ScoreDoc[] hits = isearcher.search(query, 1000).scoreDocs;
			}
			end = System.currentTimeMillis();
			in.close();
             ireader.close();
            indexDirectory.close();
			searchTime = (end-start);
			System.out.println("SearchTime: " + searchTime + " ms");
   }
   catch(Exception e)
   {
       e.printStackTrace();
   }
	//System.out.println("IndexTime: " + indexTime + " ms");
        //System.out.println("IndexSize: " + indexSize + " kB");
   }
   
}
