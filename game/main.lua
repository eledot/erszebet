#!/usr/bin/env lua-5.1

function g_shutdown()
end

print("initializing...")

function g_mouse_event(button, down, x, y)
   if button == 1 then
      if down > 0 then
         local ents = phys_point_query({x, y})
         if ents[1] then
            print("clicked!")
         end
      else
         print("!")
      end
   end
end

function g_touchpad_event(button, down, x, y)
   if down < 0 then
      print("moving to", y, x)
   else
      g_mouse_event(button, down, y, x)
   end
end

function spawn()
   local e = { }
   ent_spawn(e)
   return e
end

function remove(e)
   if e.remove then
      e.remove(e)
   end
   print(e)
   ent_remove(e)
end

phys_set_gravity(0.0)
phys_set_speed(0.25)

function spawn_base(origin, team)
   local b = spawn()
   phys_set_body(b, BODY_CIRCLE, 20)
   b.classname = "base"
   b.mass = 1
   b.gravity = 0
   b.team = team
   ent_set_sprite(b, "base")
   b.touch = function (self, other, origin, normal)
                return 0
             end

   b.think = function (self)
                b.frame = b.frame + 1
                b.nextthink = time + 0.01
             end

   b.draw = function (self)
            end

   b.nextthink = time + 0.1
   b.origin = origin
end

spawn_base({ 190, 160 }, 0)

print("done")
