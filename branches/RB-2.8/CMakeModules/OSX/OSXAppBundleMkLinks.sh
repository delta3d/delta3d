for token in *.app ; do 
   pushd $token/Contents; 
   ln -sf ../../../Frameworks;
   ln -sf ../../../lib ; 
   mkdir PlugIns; 
   pushd PlugIns; 
   ln -sf ../../../../lib/osgPlugins-3.2.1;
   ln -sf ../../../../lib/stplugins;
   ln -sf ../../../../lib/directorplugins;
   ln -sf ../../../../PalPlugins;
   ln -sf ../../../../QtPlugins/imageformats;
   popd;
   pushd Resources; 
   rm -f deltaData;
   mkdir deltaData;
   cp -r ../../../../data/* deltaData;
   ln -sf ../../../../examples;
   popd;
   popd;
done
