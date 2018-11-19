dbus-send --print-reply --dest=com.github.gyan000.eos-connect               \
          /com/github/gyan000/eosconnect/share                              \
          com.github.gyan000.eosconnect.Share.Files                         \
          string:'fffefc6422d1dda0'                                         \
          string:'/home/gyan000e/send-to.jpeg /home/gyan000e/k-yuko-contacts.csv'






dbus-send --dest=org.freedesktop.ExampleName               \
                /org/freedesktop/sample/object/name              \
                org.freedesktop.ExampleInterface.ExampleMethod   \
                int32:47 string:'hello world' double:65.32       \
                array:string:"1st item","next item","last item"  \
                dict:string:int32:"one",1,"two",2,"three",3      \
                variant:int32:-8                                 \
                objpath:/org/freedesktop/sample/object/name
