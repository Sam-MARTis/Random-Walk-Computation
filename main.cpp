#include <iostream>
#include "kernel.hpp"
#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cstdint>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdint>
#include <vtkAutoInit.h>
#include <vtkTextProperty.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

struct Config
{
    float dt;
    float T;
    float trajectories_count;
    float Force_Amplitude;
};

void perform_compatibility_check()
{
    return;
}
void parse_arguments(int argc, char **argv, float &dt, float &T, int &n, float &A)
{

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "--dt" && i + 1 < argc)
        {
            dt = std::stof(argv[++i]);
        }
        else if (arg == "--T" && i + 1 < argc)
        {
            T = std::stof(argv[++i]);
        }
        else if (arg == "--n" && i + 1 < argc)
        {
            n = std::stoi(argv[++i]);
        }
        else if (arg == "--A" && i + 1 < argc)
        {
            A = std::stof(argv[++i]);
        }
    }
}

void save_data_to_file(const float *data, const int n, const int nt, const float T, const float dt)
{
    std::ofstream out("particles.txt");
    out << n << ' ' << nt << ' ' << T << ' ' << dt << '\n';
    for (size_t i = 0; i < (size_t)n; ++i)
    {
        for (size_t t = 0; t < (size_t)nt; ++t)
            out << data[i * nt + t] << ' ';
        out << '\n';
    }
    out.close();
}

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkCubeAxesActor.h>
#include <vtkCamera.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

void plot_data_vtk(
    const float *data,
    int n,
    int nt,
    float T,
    float dt)
{
    auto points = vtkSmartPointer<vtkPoints>::New();
    auto lines = vtkSmartPointer<vtkCellArray>::New();

    vtkIdType point_id = 0;

    for (int i = 0; i < n; ++i)
    {
        auto polyline = vtkSmartPointer<vtkPolyLine>::New();
        polyline->GetPointIds()->SetNumberOfIds(nt);

        for (int t = 0; t < nt; ++t)
        {
            float y = data[i * nt + t];
            float x = t * dt;
            float z = 0.0f;

            points->InsertNextPoint(x, y, z);
            polyline->GetPointIds()->SetId(t, point_id++);
        }

        lines->InsertNextCell(polyline);
    }

    auto polydata = vtkSmartPointer<vtkPolyData>::New();
    /* ---------- Transform (scaling + shift) ---------- */
    auto transform = vtkSmartPointer<vtkTransform>::New();

    /* Example scaling (choose one) */
    // transform->Scale(0.2, 1.0, 1.0);   // X compressed
    transform->Scale(1.5, 0.5, 1.0); // X stretched, Y compressed

    /* Optional: move plot left */
    double bounds0[6];
    polydata->GetBounds(bounds0);
    double x_shift = 0.1 * (bounds0[1] - bounds0[0]);
    transform->Translate(-x_shift, 0.0, 0.0);

    /* Apply transform */
    auto tf = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    tf->SetInputData(polydata);
    tf->SetTransform(transform);
    tf->Update();

    polydata->SetPoints(points);
    polydata->SetLines(lines);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

    mapper->SetInputConnection(tf->GetOutputPort());

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetLineWidth(0.2);
    actor->GetProperty()->SetColor(0.0, 0.0, 1.0);

    // actor->SetScale(0.2, 1.0, 1.0); // stretch Y
    // actor->SetScale(1.5, 0.5, 1.0); // compress X
    /* ---------- Renderer FIRST ---------- */
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->SetBackground(0.1, 0.1, 0.1);

    /* ---------- Axes AFTER renderer ---------- */
    auto axes = vtkSmartPointer<vtkCubeAxesActor>::New();
    double bounds[6];
    tf->GetOutput()->GetBounds(bounds);
    axes->SetBounds(bounds);

    axes->SetCamera(renderer->GetActiveCamera());

    axes->GetTitleTextProperty(0)->SetColor(1, 1, 1);
    axes->GetTitleTextProperty(1)->SetColor(1, 1, 1);
    axes->GetTitleTextProperty(2)->SetColor(1, 1, 1);

    axes->GetLabelTextProperty(0)->SetColor(1, 1, 1);
    axes->GetLabelTextProperty(1)->SetColor(1, 1, 1);
    axes->GetLabelTextProperty(2)->SetColor(1, 1, 1);

    axes->DrawXGridlinesOn();
    axes->DrawYGridlinesOn();
    axes->DrawZGridlinesOff();

    renderer->AddActor(axes);

    /* ---------- Window / Interactor ---------- */
    auto window = vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer);
    window->SetSize(1200, 800);

    auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(window);

    window->SetWindowName("VTK Trajectories");
    window->Render();

    /* ---------- Camera control ---------- */
    renderer->ResetCamera();
    auto cam = renderer->GetActiveCamera();

    // double bounds[6];
    tf->GetOutput()->GetBounds(bounds);


    double x_min = bounds[0];
    double x_max = bounds[1];

    cam->SetPosition(
        0.5 * (x_min + x_max),
        0.5 * (bounds[2] + bounds[3]),
        2.0 * (x_max - x_min));

    cam->SetFocalPoint(
        0.5 * (x_min + x_max),
        0.5 * (bounds[2] + bounds[3]),
        0.0);

    cam->SetViewUp(0, 1, 0);
    renderer->ResetCameraClippingRange();

    interactor->Initialize();
    interactor->Start();
}

#define DEFAULT_DT 0.01f
#define DEFAULT_T 10.0f
#define DEFAULT_N 1000
#define DEFAULT_A 1.0f

int main(int argc, char **argv)
{

    float dt = DEFAULT_DT, T = DEFAULT_T, A = DEFAULT_A;
    int n = DEFAULT_N;
    parse_arguments(argc, argv, dt, T, n, A);
    perform_compatibility_check();

    const int nt = static_cast<int>(T / dt);
    float *results = (float *)malloc(n * nt * sizeof(float));

    kernel_launcher(results, dt, T, n, A);

    // save_data_to_file(results, n, nt, T, dt);
    plot_data_vtk(results, n, nt, T, dt);
    std::cout << "Simulation completed. Results are visualized using VTK.\n";

    free(results);
    return 0;
}