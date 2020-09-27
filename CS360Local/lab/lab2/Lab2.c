//Lab2
#include <stdio.h>             // for I/O
#include <stdlib.h>            // for lib functions
#include <libgen.h>            // for dirname()/basename()
#include <string.h>

typedef struct node{
          char  pathname[64];
          char  name[64];       // node's name string
          char  type;
   struct node *child, *sibling, *parent;
}NODE;


NODE *root, *cwd, *start;
char command[16], globalPathname[64];

char gpath[128];               // global gpath[] to hold token strings
char *name[64];                // token string pointers
int  n;                        // number of token strings

char dname[64], bname[64];     // dirname, basename of pathname

//               0       1      2       3       4         5      6      7        8       9
char *cmd[] = {"mkdir", "ls", "quit", "pwd", "rmdir", "creat", "rm", "save", "reload", "cd",0};

int findCmd(char *command)
{
   int i = 0;
   while(cmd[i]){
     if (strcmp(command, cmd[i])==0)
         return i;
     i++;
   }
   return -1;
}

NODE *search_child(NODE *parent, char *name)
{
  NODE *p;
  printf("search for %s in parent DIR:%s\n", name, parent->name);
  p = parent->child;
  if (p==0)
    return 0;
  while(p)
  {
    if (strcmp(p->name, name)==0)
      return p;
    p = p->sibling;
  }
  return 0;
}

int insert_child(NODE *parent, NODE *q)
{
  NODE *p;
  printf("insert NODE %s into parent %s child list\n", q->name, parent->name);
  p = parent->child;
  if (p==0)
    parent->child = q;
  else{
    while(p->sibling)
      p = p->sibling;
    p->sibling = q;
  }
  q->parent = parent;
  q->child = 0;
  q->sibling = 0;
}

int tokenize(char *pathname)
{
   // Divide pathname into token strings in gpath[128]
   // Let char * name[0], name[1], ..., name[n-1] point at token strings
   // Let n = number of token strings
   // print out token strings to verify
   int n=0;
   char *divPathName;
   divPathName = strtok(pathname, "/");
   while(divPathName)
   {
     name[n] = divPathName;
     n++;
     divPathName = strtok(0, "/");
   }
   return n;
}

NODE* path2node(char *pathname)
{
   // return pointer to the node of pathname, or NULL if invalid
   // pathanme begins with /
   if(*pathname == '.')
   {
     return root;
   }

   if (*pathname == '/')
      start = root;
   else
      start = cwd;

   n = tokenize(pathname);
   NODE *node = start;
  
   for (int i=0; i<n; i++){
       node = search_child(node, name[i]);
       //if search fails, return NULL;
       if (!node)
       {
         printf("%s search failed!\n", name[i]);
         return NULL;
       }
   }
   return node;
}

int dir_base_name(char *pathname)
{    
    char temp[128];
    strcpy(temp, pathname);
    strcpy(dname, dirname(temp));
    strcpy(temp, pathname);
    strcpy(bname, basename(temp));
}

/***************************************************************
 This mkdir(char *name) makes a DIR in the current directory
 You MUST improve it to mkdir(char *pathname) for ANY pathname
****************************************************************/

int mkdir(char *pathname)
{
  NODE *p, *q;
  printf("mkdir: name=%s\n", pathname);

  if(pathname[0] == NULL)
  {
      printf("Directory name should not be empty.\n");
      return -1;
  }

  // write YOUR code to not allow mkdir of /, ., ./, .., or ../
  char *invalidName1 = "/", *invalidName2 = ".", 
       *invalidName3 = "./", *invalidName4 = "..",
       *invalidName5 = "../";
  
  if(strcmp(pathname, invalidName1) == 0|| 
  strcmp(pathname, invalidName2) == 0|| 
  strcmp(pathname, invalidName3) == 0|| 
  strcmp(pathname, invalidName4) == 0||
  strcmp(pathname, invalidName5) == 0)
  {
    //if dirname is invalid
    //return -1
    printf("Invalid pathname!\n");
    return -1;
  }

  // get dirname and basename
  dir_base_name(pathname);

  if (pathname[0]=='/')
    // absolute pathname
    start = root;
  else
    // relative pathname
    start = cwd;

  // search for the dirname node
  
  printf("check whether %s already exists\n", dname);
  //char *dirName = &dname[0];

  // set p to directory node
  p = path2node(dname);
  
  // if p exist and is Dir type
  if (p && p->type == 'D')
  {
    printf("--------------------------------------\n");
    printf("ready to mkdir %s\n", bname);
    //if already exists: error message and return FAIL
    char *baseName = &bname[0];
    if(search_child(p, bname))
    {
      printf("%s already exists!mkdir failed\n", bname);
      return -1;
    }
    //add a new node 
    q = (NODE *)malloc(sizeof(NODE));
    q->type = 'D';
    strcpy(q->name, bname);
    strcpy(q->pathname, pathname);
    insert_child(p, q);
    printf("mkdir %s OK\n", bname);
    printf("--------------------------------------\n");
    return 1; 
  }
  else
  {
    printf("%s is not valid! Check if this is a valid directory\n", dname);
    return -1;
  }
}

// This ls() list CWD. You MUST improve it to ls(char *pathname)
int ls(char* pathname)
{
  NODE *p;
  if(*pathname == NULL)
  {
    // if no pathname input, print cwd
    p = cwd->child;
    printf("cwd %s contents = ", cwd->name);
    while (p){
      printf("[%c %s] ", p->type, p->name);
      p = p->sibling;
    }
    printf("\n");
  }
  else
  {
    // if there is pathname input, check if this is a valid directory
    if(pathname[0] == '/')
    {
      // pathname is absolute
      start = root;
    }
    else
    {
      // pathname is relative
      start = cwd;
    }

    // move to consponding node
    p = path2node(pathname);

    if(p == 0)
    {
      // if no search path
      printf("%s doesn't exist!\n", pathname);
      return -1;
    }
    else
    {
      // find pathname, point p to pathname child
      p = p->child;
      printf("cwd %s contents = ", p->parent->name);
      while (p)
      {
        printf("[%c %s] ", p->type, p->name);
        p = p->sibling;
        
      }
      printf("\n");
    }
  }
  return 1;
}

int quit()
{
  printf("Program exit\n");
  save();
  exit(0);
  
  // improve quit() to SAVE the current tree as a Linux file
  // for reload the file to reconstruct the original tree
}

int initialize()  // create / node, set root and cwd pointers
{
    root = (NODE *)malloc(sizeof(NODE));
    strcpy(root->name, "/");
    strcpy(root->pathname, "/");
    root->parent = root;
    root->sibling = 0;
    root->child = 0;
    root->type = 'D';
    cwd = root;
    printf("Root initialized OK\n");
}

int main()
{
  int index;
  char line[128];
  
  initialize();

  printf("NOTE: commands = [mkdir|ls|quit|creat|pwd|rmdir|rm|save|reload]\n");

  while(1){
      printf("Enter command line : ");
      fgets(line, 128, stdin);
      line[strlen(line)-1] = 0;

      sscanf(line, "%s %s", command, globalPathname);
      printf("command=%s pathname=%s\n", command, globalPathname);
      
      if (command[0]==0) 
         continue;

      index = findCmd(command);

      switch (index)
      {
        case 0: mkdir(globalPathname); break;
        case 1: ls(globalPathname);    break;
        case 2: quit();          break;
        case 3: pwd(cwd);printf("\n");       break;
        case 4: rmdir(globalPathname); break;
        case 5: creat(globalPathname); break;
        case 6: rm(globalPathname); break;
        case 7: save(); break;
        case 8: reload(); break;
        case 9: cd(globalPathname); break;
      }
      // empty gloablPathname
      memset(globalPathname, 0, 64);
  }
}


/********************* YOU DO THE FOLLOWING *************************/    
//Write your pwd(), rmdir(), creat(), rm(), save(), reload() functions
/************************************************************************/
int creat(char* pathname)
{
  NODE *p, *q;
  printf("mkdir: name=%s\n", pathname);

  if(pathname[0] == NULL)
  {
      printf("Directory name should not be empty.\n");
      return -1;
  }

  // write YOUR code to not allow mkdir of /, ., ./, .., or ../
  char *invalidName1 = "/", *invalidName2 = ".", 
       *invalidName3 = "./", *invalidName4 = "..",
       *invalidName5 = "../";
  
  if(strcmp(pathname, invalidName1) == 0|| 
  strcmp(pathname, invalidName2) == 0|| 
  strcmp(pathname, invalidName3) == 0|| 
  strcmp(pathname, invalidName4) == 0||
  strcmp(pathname, invalidName5) == 0)
  {
    //if dirname is invalid
    //return -1
    printf("Invalid pathname!\n");
    return -1;
  }

  // get dirname and basename
  dir_base_name(pathname);

  if (pathname[0]=='/')
    // absolute pathname
    start = root;
  else
    // relative pathname
    start = cwd;

  // search for the dirname node
  
  printf("check whether %s already exists\n", dname);
  //char *dirName = &dname[0];

  // set p to directory node
  p = path2node(dname);
  
  // if p exist and is Dir type
  if (p && p->type == 'D')
  {
    printf("--------------------------------------\n");
    printf("ready to mkdir %s\n", bname);
    //if already exists: error message and return FAIL
    char *baseName = &bname[0];
    if(search_child(p, bname))
    {
      printf("%s already exists!mkdir failed\n", bname);
      return -1;
    }
    //add a new node 
    q = (NODE *)malloc(sizeof(NODE));
    q->type = 'F';
    strcpy(q->name, bname);
    strcpy(q->pathname, pathname);
    insert_child(p, q);
    printf("mkdir %s OK\n", bname);
    printf("--------------------------------------\n");
    return 1; 
  }
  else if(p == 0)
  {
    // if no p
    printf("--------------------------------------\n");
    printf("ready to mkdir %s\n", bname);
    q = (NODE *)malloc(sizeof(NODE));
    q->type = 'F';
    strcpy(q->name, bname);
    insert_child(start, q);
    printf("mkdir %s OK\n", bname);
    printf("--------------------------------------\n");
    return 1; 
  }
  else
  {
    printf("%s is not valid! Check if this is a valid directory\n", dname);
    return -1;
  }
}

int rmdir(char *pathname)
{
  NODE *p;
  if(pathname[0] == NULL)
  {
      printf("Directory name should not be empty.\n");
      return -1;
  }

  // write YOUR code to not allow mkdir of /, ., ./, .., or ../
  char *invalidName1 = "/", *invalidName2 = ".", 
       *invalidName3 = "./", *invalidName4 = "..",
       *invalidName5 = "../";
  
  if(strcmp(pathname, invalidName1) == 0|| 
  strcmp(pathname, invalidName2) == 0|| 
  strcmp(pathname, invalidName3) == 0|| 
  strcmp(pathname, invalidName4) == 0||
  strcmp(pathname, invalidName5) == 0)
  {
    //if dirname is invalid
    //return -1
    printf("Invalid pathname!\n");
    return -1;
  }

  if(pathname[0] == '/')
  {
    // pathname is absolute
    start = root;
  }
  else
  {
    // pathname is relative
    start = cwd;
  }

  // get node want to be removed
  p = path2node(pathname);

  if(p == 0)
  {
    // path doesn't exist
    printf("%s is not a existed directory! Deletion failed\n", pathname);
    return -1;
  }
  else if(p->type != 'D')
  {
    // try to delete none Dir type node.
    printf("%s is not a DIR, try other command to remove it.\n", pathname);
    return -1;
  }
  else
  {
    // if node exists
    NODE *pParent = p->parent;
    // remove p child and sibling
    printf("Remove node %s child and sibling\n", p->name);
    p->child = NULL;
    // remove p from its parent
    printf("Remove node %s from its parent node: %s\n", p->name, pParent->name);
    pParent->child = p->sibling;
    // free p space
    char *pName = p->name;
    printf("Free node %s space\n", pName);
    free(p);

    // check if the remove is successful
    if(path2node(pathname) == 0)
    {
      printf("Remove node %s successfully!\n", pathname);
      return 1;
    }
    else
    {
      printf("Remove node %s failed!\n", pathname);
      return -1;
    }
  }
}

int rm(char *pathname)
{
  NODE *p;
  if(pathname[0] == NULL)
  {
      printf("Directory name should not be empty.\n");
      return -1;
  }

  // write YOUR code to not allow mkdir of /, ., ./, .., or ../
  char *invalidName1 = "/", *invalidName2 = ".", 
       *invalidName3 = "./", *invalidName4 = "..",
       *invalidName5 = "../";
  
  if(strcmp(pathname, invalidName1) == 0|| 
  strcmp(pathname, invalidName2) == 0|| 
  strcmp(pathname, invalidName3) == 0|| 
  strcmp(pathname, invalidName4) == 0||
  strcmp(pathname, invalidName5) == 0)
  {
    //if dirname is invalid
    //return -1
    printf("Invalid pathname!\n");
    return -1;
  }

  if(pathname[0] == '/')
  {
    // pathname is absolute
    start = root;
  }
  else
  {
    // pathname is relative
    start = cwd;
  }

  p = path2node(pathname);

  if(p->type != 'F')
  {
    // try to delete none Dir type node.
    printf("%s is not a File, try other command to remove it.\n", pathname);
    return -1;
  }
  else
  {
    // if node exists
    NODE *pParent = p->parent;
    // remove p from its parent
    printf("Remove node %s from its parent node: %s\n", p->name, pParent->name);
    pParent->child = NULL;
    // free p space
    char *pName = p->name;
    printf("Free node %s space\n", pName);
    free(p);

    // check if the remove is successful
    if(!pParent->child)
    {
      printf("Remove node %s successfully!\n", pathname);
      return 1;
    }
    else
    {
      printf("Remove node %s failed!\n", pathname);
      return -1;
    }
  }
}

void pwd(NODE *curNode)
{
  char *curNodeName = curNode->name;
  // if not root, keep going back
  if(curNode != root)
  {
    pwd(curNode->parent);
  }
  printf("/%s", curNodeName);
}

int cd(char *pathname)
{
  if(*pathname == 0)
  {
    // if given pathname is empty
    pathname = cwd->pathname;
  }

  if(pathname[0] == NULL)
  {
      printf("Directory name should not be empty.\n");
      return -1;
  }

  // write YOUR code to not allow mkdir of /, ., ./, .., or ../
  char *invalidName1 = "/", *invalidName2 = ".", 
       *invalidName3 = "./",*invalidName5 = "../";
  
  if(strcmp(pathname, invalidName1) == 0|| 
  strcmp(pathname, invalidName2) == 0|| 
  strcmp(pathname, invalidName3) == 0|| 
  strcmp(pathname, invalidName5) == 0)
  {
    //if dirname is invalid
    //return -1
    printf("Invalid pathname!\n");
    return -1;
  }

  // if pathname is .. command
  char *backCommand = "..";
  if(strcmp(pathname, backCommand)==0)
  {
    if(cwd->parent!=0)
    {
      cwd = cwd->parent;
      return 1;
    }
    else
    {
      printf("Can't go back over %s node. Go back failed\n", cwd->name);
      return -1;
    }
  }
  // actual cd
  NODE *p = path2node(pathname);
  if(p == 0)
  {
    printf("can't reach this directory! Make sure it exists\n");
    return -1;
  }

  if(p->type == 'D')
  {
    cwd = p;
    printf("cwd:%s\n", p->name);
    return 1;
  }
  else
  {
    printf("%s is an Invalid directory name!\n", pathname);
    return -1;
  }
}

void writeTreeToFile(FILE *file, NODE *curNode)
{
  if(curNode != NULL)
  {
    fprintf(file, "%c %s\n", curNode->type, curNode->pathname);
    writeTreeToFile(file, curNode->child);
    writeTreeToFile(file, curNode->sibling);
  }
}

int save()
{
  FILE *fp = fopen("myTree", "w+");
  writeTreeToFile(fp, root);
  if(fclose(fp) == NULL)
  {
    printf("file closed.Hope it write tree correctly\n");
  }
  return 1;
}

void desotryTree(NODE *curNode)
{
  if(curNode->child)
  {
    // destory child
    desotryTree(curNode->child);
  }
  if(curNode->sibling)
  {
    // destory sibling
    desotryTree(curNode->sibling);
  }
  curNode->child = NULL;
  curNode->sibling = NULL;
  free(curNode);
}

int reload()
{
  char newNodePath[128];
  // destory old tree
  desotryTree(root);
  char *nodeName;
  FILE *fp = fopen("myTree", "r");
  initialize();
  // skip the first line of file
  fgets(newNodePath, 128, fp);
  // read the rest of the file
  while(fgets(newNodePath, 128, fp)!=NULL)
  {
    nodeName = strtok(newNodePath, " ");
    if(newNodePath[0] == 'D')
    {
      nodeName = strtok(NULL, "\n");
      mkdir(nodeName);
    }
    else if(newNodePath[0] == 'F')
    {
      nodeName = strtok(NULL, "\n");
      creat(nodeName);
    }
  }
  fclose(fp);
}