#include <stdio.h>  

#include <string.h>    

#include <curl/curl.h>

#include "cm_http_upload.h"

/*
 * @url: ip:port
 */
int cm_http_send_file(char *filename, char *url, int subid)
{
  CURL *curl;  
  CURLcode res;  
  
  struct curl_httppost *formpost=NULL;  
  struct curl_httppost *lastptr=NULL;  
  struct curl_slist *headerlist=NULL;  
  static const char buf[] = "Expect:";  

  int ret = 0;
  // only linux
  char *p = rindex(filename, '/');
  if (!p) p = filename;
  /* Fill in the file upload field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "logFile",
               CURLFORM_FILE, filename,  
               CURLFORM_END);

  char subidstr[8] = {0};
  sprintf(subidstr, "%d", subid);
  if (subid != 0) {
      curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "subId",
                 CURLFORM_COPYCONTENTS, subidstr,
                 CURLFORM_END);
  }
  
  /* Fill in the filename field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "logFile",
               CURLFORM_COPYCONTENTS, p,  
               CURLFORM_END);  
  
  /* Fill in the submit field too, even if this is rarely needed */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "submit",  
               CURLFORM_COPYCONTENTS, "Submit",  
               CURLFORM_END);

  curl = curl_easy_init();  
  /* initalize custom header list (stating that Expect: 100-continue is not 
     wanted */  
  headerlist = curl_slist_append(headerlist, buf);  
  if(curl) {  
    /* what URL that receives this POST */  
	  char fullurl[512] = {0};
	  sprintf(fullurl, "http://%s", url);
     curl_easy_setopt(curl, CURLOPT_URL, fullurl);  
    //if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )  
      /* only disable 100-continue header if explicitly requested */  
     // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);  
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
  
    /* Perform the request, res will get the return code */  
    res = curl_easy_perform(curl);  
    /* Check for errors */  
    if(res != CURLE_OK) {
      ret = -1;
    }
  
    /* always cleanup */  
    curl_easy_cleanup(curl);  
  
    /* then cleanup the formpost chain */  
    curl_formfree(formpost);  
    /* free slist */  
    curl_slist_free_all (headerlist);  
  } 

  return ret;
}

/*
int main(int argc, char *argv[])  
{  
  
  curl_global_init(CURL_GLOBAL_ALL);  
  
  cm_http_send_file("cm_print.h", "192.168.5.138");
  
  return 0;  
}
 */
