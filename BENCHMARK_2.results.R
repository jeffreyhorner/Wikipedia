library(ggplot2)
library(tidyr)
library(dplyr)
library(RColorBrewer)
library(dichromat)

d <- read.csv('BENCHMARK_2.results.csv',header=FALSE,sep='\t',stringsAsFactors=FALSE)
names(d) <- c('Program','file','tokens','construct','search')
d <- d %>% 
    group_by(Program,file) %>% 
    summarise(Construct = min(construct), Search=min(search)) %>%
    extract(file,c('filetype','filelines'),'(\\w+).(\\d)mil') %>%
    mutate(lines=as.integer(filelines)*10^6) %>%
    gather(time_type,time,Construct,Search) %>%
    select(-filelines)
d$Program <- sub('.*(R-3.2.2).*','\\1',d$Program)
d$Program <- sub('.*(R-Array-Hash).*','\\1',d$Program)
d$Program <- sub('^C$','C-Array-Hash',d$Program)
d$Program <- sub('^python$','Python-Dict',d$Program)
d$Program <- sub('^julia$','Julia-Dict',d$Program)

prog_color <- brewer.pal(n=12,name='Paired')[c(1,2,5,6,9,10)]
names(prog_color) <- c('C-Array-Hash','C-Dense-Hash','Python-Dict','Julia-Dict','R-Array-Hash','R-3.2.2')

prog_sub <- c('C-Array-Hash','Python-Dict','C-Dense-Hash')
py_c_dist <- 
  ggplot(
    d %>% filter(filetype=='DISTINCT',Program %in% prog_sub),
    aes(x=lines,y=time)
  ) +
	geom_line(aes(colour=Program)) +
	geom_point(aes(colour=Program)) + 
	labs(x='',y='',title='DISTINCT') +
	theme(
	    axis.text = element_text(colour = "black")
	) +
  scale_color_manual(values=prog_color) +
  facet_grid (. ~ time_type)

png(filename='C_Python_Distinct.png',width=600,height=300)
print(py_c_dist)
dev.off()

py_c_skew <- 
  ggplot(
    d %>% filter(filetype=='SKEW',Program %in% prog_sub),
    aes(x=lines,y=time)
  ) +
	geom_line(aes(colour=Program)) +
	geom_point(aes(colour=Program)) + 
	labs(x='',y='',title='SKEW') +
	theme(
	    axis.text = element_text(colour = "black")
	) +
  scale_color_manual(values=prog_color) +
  facet_grid (. ~ time_type)

png(filename='C_Python_Skew.png',width=600,height=300)
print(py_c_skew)
dev.off()

prog_sub <- c('C-Array-Hash','C-Dense-Hash')
c_dist <- 
  ggplot(
    d %>% filter(filetype=='DISTINCT',Program %in% prog_sub),
    aes(x=lines,y=time)
  ) +
	geom_line(aes(colour=Program)) +
	geom_point(aes(colour=Program)) + 
	labs(x='',y='',title='DISTINCT') +
	theme(
	    axis.text = element_text(colour = "black")
	) +
  scale_color_manual(values=prog_color) +
  facet_grid (. ~ time_type)

png(filename='C_Distinct.png',width=600,height=300)
print(c_dist)
dev.off()

c_skew <- 
  ggplot(
    d %>% filter(filetype=='SKEW',Program %in% prog_sub),
    aes(x=lines,y=time)
  ) +
	geom_line(aes(colour=Program)) +
	geom_point(aes(colour=Program)) + 
	labs(x='',y='',title='SKEW') +
	theme(
	    axis.text = element_text(colour = "black")
	) +
  scale_color_manual(values=prog_color) +
  facet_grid (. ~ time_type)

png(filename='C_Skew.png',width=600,height=300)
print(c_skew)
dev.off()

all_dist <- 
  ggplot(
    d %>% filter(filetype=='DISTINCT'),
    aes(x=lines,y=time)
  ) +
  labs(
    title="Performance of Hash Table with Distinct Keys",
    x="Number of Elements in Table",
    y="Time in Seconds") +
  geom_line(aes(colour=Program)) +
	geom_point(aes(colour=Program)) + 
	theme(
	    axis.text = element_text(colour = "black")
	) +
  scale_color_manual(values=prog_color) +
  scale_y_log10(breaks=c(0.5,1,5,50,100,150,1000)) +
  facet_grid (. ~ time_type)

png(filename='All_Distinct.png',width=600,height=400)
print(all_dist)
dev.off()
