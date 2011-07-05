[a,err]=file('open', getenv("HOME") + '/powerdevs/bin/BackDoor/loader.sce',"old");
if err==240 then
  exec(getenv("HOME") + '/powerdevs/bin/BackDoor/builder.sce','errcatch');
  exec(getenv("HOME") + '/powerdevs/bin/BackDoor/builder.sce');
end
exec(getenv("HOME") + '/powerdevs/bin/BackDoor/loader.sce');
cd(getenv("HOME") + '/powerdevs/output');
