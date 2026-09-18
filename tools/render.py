#!/usr/bin/env python3
import argparse
import json
from pathlib import Path
import webbrowser

import plotly.graph_objects as go


def xyz(points):
    if not points:
        return [], [], []
    x, y, z = zip(*points)
    return list(x), list(y), list(z)


def build_figure(run):
    world = run["world"]
    width = int(world["width"])
    height = int(world["height"])
    depth = int(world["depth"])

    fig = go.Figure()

    obstacles = run.get("obstacles", [])
    ox, oy, oz = xyz(obstacles)
    fig.add_trace(
        go.Scatter3d(
            x=ox, y=oy, z=oz,
            mode="markers",
            name="Blocked",
            marker=dict(size=5, symbol="square", opacity=0.35),
            hovertemplate="blocked (%{x}, %{y}, %{z})<extra></extra>",
        )
    )

    goals = run.get("goals", [])
    gx, gy, gz = xyz(goals)
    fig.add_trace(
        go.Scatter3d(
            x=gx, y=gy, z=gz,
            mode="markers",
            name="Goals",
            marker=dict(size=8, symbol="diamond"),
            hovertemplate="goal (%{x}, %{y}, %{z})<extra></extra>",
        )
    )

    # Planned paths are static; moving agent markers are animated over them.
    for team in run.get("teams", []):
        team_id = team["id"]
        for agent in team.get("agents", []):
            path = agent.get("path", [])
            px, py, pz = xyz(path)
            fig.add_trace(
                go.Scatter3d(
                    x=px, y=py, z=pz,
                    mode="lines",
                    name=f"T{team_id} A{agent['id']} path",
                    line=dict(width=5),
                    hoverinfo="skip",
                    showlegend=False,
                )
            )

    frames = run.get("frames", [])
    initial = frames[0]["agents"] if frames else []

    def agent_arrays(items):
        positions = [a["position"] for a in items]
        ax, ay, az = xyz(positions)
        labels = [f"team {a['team']} agent {a['agent']}" for a in items]
        teams = [a["team"] for a in items]
        
        return ax, ay, az, labels, teams

    ax, ay, az, labels, team_values = agent_arrays(initial)
    agent_trace_index = len(fig.data)
    fig.add_trace(
        go.Scatter3d(
            x=ax, y=ay, z=az,
            mode="markers",
            name="Agents",
            text=labels,
            marker=dict(
                size=8,
                color=team_values,
                colorscale="Turbo",
                showscale=False,
            ),
            hovertemplate="%{text}<br>(%{x}, %{y}, %{z})<extra></extra>",
        )
    )

    animation_frames = []
    for frame in frames:
        fx, fy, fz, flabels, fteams = agent_arrays(frame.get("agents", []))
        animation_frames.append(
            go.Frame(
                name=str(frame["step"]),
                traces=[agent_trace_index],
                data=[
                    go.Scatter3d(
                        x=fx, y=fy, z=fz,
                        mode="markers",
                        text=flabels,
                        marker=dict(
                            size=8,
                            color=fteams,
                            colorscale="Turbo",
                            showscale=False,
                        ),
                        hovertemplate="%{text}<br>(%{x}, %{y}, %{z})<extra></extra>",
                    )
                ],
            )
        )

    fig.frames = animation_frames

    slider_steps = [
        dict(
            method="animate",
            label=str(frame["step"]),
            args=[[str(frame["step"])], {
                "mode": "immediate",
                "frame": {"duration": 0, "redraw": True},
                "transition": {"duration": 0},
            }],
        )
        for frame in frames
    ]

    fig.update_layout(
        title=f"Span — {run.get('mission', 'unknown')} / {run.get('planner', 'unknown')}",
        scene=dict(
            xaxis=dict(title="X", range=[-0.5, width - 0.5]),
            yaxis=dict(title="Y", range=[-0.5, height - 0.5]),
            zaxis=dict(title="Z", range=[-0.5, depth - 0.5]),
            aspectmode="data",
        ),
        margin=dict(l=0, r=0, b=0, t=50),
        updatemenus=[
            dict(
                type="buttons",
                showactive=False,
                buttons=[
                    dict(
                        label="Play",
                        method="animate",
                        args=[None, {
                            "frame": {"duration": 350, "redraw": True},
                            "transition": {"duration": 0},
                            "fromcurrent": True,
                            "mode": "immediate",
                        }],
                    ),
                    dict(
                        label="Pause",
                        method="animate",
                        args=[[None], {
                            "frame": {"duration": 0, "redraw": False},
                            "transition": {"duration": 0},
                            "mode": "immediate",
                        }],
                    ),
                ],
            )
        ],
        sliders=[dict(active=0, currentvalue={"prefix": "step "}, steps=slider_steps)],
    )

    return fig


def main():
    parser = argparse.ArgumentParser(description="Render a Span run as an offline 3D Plotly animation.")
    parser.add_argument("run", type=Path, help="Span run JSON file")
    parser.add_argument("-o", "--output", type=Path, help="Output HTML file")
    parser.add_argument("--no-open", action="store_true", help="Do not open the generated HTML")
    args = parser.parse_args()

    with args.run.open("r", encoding="utf-8") as handle:
        run = json.load(handle)

    fig = build_figure(run)
    output = args.output or args.run.with_suffix(".html")
    fig.write_html(output, include_plotlyjs=True, auto_play=False)
    print(f"wrote {output}")

    if not args.no_open:
        webbrowser.open(output.resolve().as_uri())


if __name__ == "__main__":
    main()
