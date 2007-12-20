landareas = shaperead('world','UseGeoCoords',true);
MapAxisHandle = axesm('lambert','Grid','on','MapParallels',[50 50],'Origin',[-107]);
[x y] = mfwdtran([31; 45], [-107.5; -58]);
set(MapAxisHandle,'XLim',[x(1) x(2)],'YLim',[y(1) y(2)]);
geoshow(landareas,'FaceColor',[1 1 .5],'EdgeColor',[.6 .6 .6]);