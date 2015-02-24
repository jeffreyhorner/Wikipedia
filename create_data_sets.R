# The WIKIDUMP file is over 11G. You will want to download it yourself, preferably
# with a bittorrent client, from:
# http://meta.wikimedia.org/wiki/Data_dumps
#
WIKIDUMP <- 'enwiki-20150205-pages-articles.xml.bz2'
WIKILINES <- 10^8
dirs <- as.character(outer(LETTERS,LETTERS,paste,sep=''))
unlink(c('pages-articles.xml','SKEW','DISTINCT',dirs),recursive=TRUE)

# Tease out wiki articles
system(
  sprintf("bzcat %s | head -n %d > pages-articles.xml",
        WIKIDUMP, WIKILINES)
)
system("./wikiextractor.py -b 512M < pages-articles.xml")

invisible(lapply(dirs, function(d){
  if (file.exists(d))
    lapply(dir(d),function(k){
      command <- sprintf("tokenize %s/%s | grep -v '^http' | ./only_ascii >> SKEW",d,k)
      cat(command,'\n')
      system(command);
    })
}))
system("./orderfreq < SKEW > SKEW.of.txt")
system("sort -n -k 1 < SKEW.of.txt | awk '{print $3}' > DISTINCT")
system("wc -l SKEW")
system("./mean_strlen < SKEW")
system("wc -l DISTINCT")
system("./mean_strlen < DISTINCT")
