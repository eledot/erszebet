#!/usr/bin/env lua-5.1

require("game/common")
require("game/base")

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

function spawn_base2(origin, team)
   local b = spawn()
   phys_set_body(b, BODY_CIRCLE, 30)
   b.classname = "base"
   b.mass = 1
   b.gravity = 0
   b.team = team
   b.flags = FL_STATIC
   ent_set_sprite(b, "base", 0)
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

function spawn_squad(origin, team)
   local s = spawn()
   phys_set_body(s, BODY_CIRCLE, 10)
   s.classname = "squad"
   s.mass = 1
   s.scale = 0.75
   s.team = team
   ent_set_sprite(s, "squad", 1)
   s.touch = function (self, other, origin, normal)
                if other.classname == "base" and not (other.team == self.team) then
                   print("base attack")
                elseif other.classname == "squad" then
                   if other.team == self.team then
                      print("joining another squad")
                   else
                      print("squad attack")
                   end
                end
                return 0
             end

   s.think = function (self)
                s.frame = s.frame + 1
                s.nextthink = time + 0.065
             end

   s.draw = function (self)
            end

   s.nextthink = time + 0.065
   s.origin = origin
end

spawn_base({ 40, 240, 0 }, 0)
spawn_base({ 340, 40, 0 }, 1)
spawn_squad({ 40, 240, 1 }, 0)
spawn_squad({ 340, 40, 1 }, 1)

execute("bind l g_list_entities")

print("done")
