# The WIKIDUMP file is over 11G. You will want to download it yourself, preferably
# with a bittorrent client, from:
# http://meta.wikimedia.org/wiki/Data_dumps
#
WIKIDUMP <- 'enwiki-20150205-pages-articles.xml.bz2'
WIKIDIR <- 'wiki_output'
unlink(c('SKEW','DISTINCT',WIKIDIR),recursive=TRUE)

# Tease out wiki articles: takes about 5.5 hours
system(sprintf("./Wikiextractor.py -f tanl -b 512M --overwrite %s %s",WIKIDUMP,WIKIDIR))

# doesn't take too terribly long
invisible(lapply(dir(WIKIDIR,'*.raw',full.names=TRUE), 
  function(d){
    token_file <- sub('.raw$','.token',d)
    command <- sprintf("tokenize %s | grep -v '^<doc\\|</doc' | ./only_ascii | grep -v 'http:\\|https:' > %s",d,token_file)
    #cat(command,'\n')
    system(command)
  }
))

# takes seconds
system(sprintf("cat %s/*.token > SKEW",WIKIDIR))

# takes about 6 minutes
system("sort -u -S 4G SKEW > DISTINCT.srt")

# takes about 14 minutes
system("./orderfreq DISTINCT.srt SKEW > SKEW.of")

system("sort -n -k 1 < SKEW.of | awk '{print $3}' > DISTINCT")
system("wc -l SKEW")
system("./mean_strlen < SKEW")
system("wc -l DISTINCT")
system("./mean_strlen < DISTINCT")

for (i in seq(1:10)) system(sprintf("head --lines=%dMB SKEW > SKEW.%dmil",i,i))
for (i in seq(1:8)) system(sprintf("head --lines=%dMB DISTINCT > DISTINCT.%dmil",i,i))
