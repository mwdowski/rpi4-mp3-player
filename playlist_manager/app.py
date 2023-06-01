import asyncio
import tornado
import tornado.web
import os
import sys


def get_track_list():
    global dir
    res = os.listdir(dir)
    res.sort()
    return res


def apply_reorder(new_order):
    global track_list
    for new_index, old_index in enumerate(new_order):
        oldname = track_list[old_index]
        newname = "{:03d}".format(new_index) + oldname[3:]
        os.rename(os.path.join(dir, oldname), os.path.join(dir, newname))
    track_list = get_track_list()


class MainHandler(tornado.web.RequestHandler):

    def get(self, query=None):
        global track_list
        self.render("playlist_view.html", track_list=track_list)

    def post(self, query):
        indices = [int(e) for e in str.split(query, ',')]
        apply_reorder(indices)


def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/([^/]+)", MainHandler),
    ])


async def main():
    app = make_app()
    app.listen(8888)
    await asyncio.Event().wait()

if __name__ == "__main__":
    if len(sys.argv) == 1:
        dir = "/home/Music"
    else:
        dir = sys.argv[1]

    track_list = get_track_list()

    asyncio.run(main())
